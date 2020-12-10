//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Win64 ELENA System Routines
//
//                                              (C)2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include <windows.h>

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

void SystemRoutineProvider::InitTLSEntry(pos_t threadIndex, pos_t tlsIndex, ProgramHeader* frameHeader, pos_t* threadTable)
{
   //TLSEntry* entry = GetTLSEntry(tlsIndex);

   //entry->tls_flags = 0;
   //entry->tls_sync_event = ::CreateEvent(0, -1, 0, 0);
   //entry->tls_et_current = &frameHeader->root_exception_struct;
   //entry->tls_threadindex = threadIndex;

   //threadTable[threadIndex] = (uintptr_t)entry;
}

void SystemRoutineProvider::Exit(pos_t exitCode)
{
   ::ExitProcess(exitCode);
}

uintptr_t SystemRoutineProvider::NewHeap(int totalSize, int committedSize)
{
   // reserve
   void* allocPtr = VirtualAlloc(nullptr, totalSize, MEM_RESERVE, PAGE_READWRITE);

   // allocate
   VirtualAlloc(allocPtr, committedSize, MEM_COMMIT, PAGE_READWRITE);

   return (uintptr_t)allocPtr;
}
