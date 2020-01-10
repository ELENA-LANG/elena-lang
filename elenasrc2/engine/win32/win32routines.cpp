//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Win32 ELENA System Routines
//
//                                              (C)2018-2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include <windows.h>

using namespace _ELENA_;

void SystemRoutineProvider :: Prepare()
{

}

void SystemRoutineProvider :: InitCriticalStruct(CriticalStruct* header, pos_t criticalHandler)
{
   pos_t previousHeader = 0;

   // ; set SEH handler / frame / stack pointers
   __asm {
      mov  eax, header
      mov  ecx, fs: [0]
      mov  previousHeader, ecx
      mov  fs : [0] , eax
   }

   header->previousStruct = previousHeader;
   header->handler = criticalHandler;
}

TLSEntry* SystemRoutineProvider :: GetTLSEntry(pos_t tlsIndex)
{
   TLSEntry* entry = nullptr;

   // ; GCXT: assign tls entry
   __asm {
      mov  ebx, tlsIndex
      mov  ecx, fs: [2Ch]
      mov  edx, [ecx + ebx * 4]
      mov  entry, edx
   }

   return entry;
}

void SystemRoutineProvider :: InitTLSEntry(pos_t threadIndex, pos_t tlsIndex, ProgramHeader* frameHeader, pos_t* threadTable)
{
   TLSEntry* entry = GetTLSEntry(tlsIndex);

   entry->tls_flags = 0;
   entry->tls_sync_event = ::CreateEvent(0, -1, 0, 0);
   entry->tls_et_current = &frameHeader->root_exception_struct;
   entry->tls_threadindex = threadIndex;

   threadTable[threadIndex] = (pos_t)entry;
}

pos_t SystemRoutineProvider :: NewHeap(int totalSize, int committedSize)
{
   // reserve
   void* allocPtr = VirtualAlloc(nullptr, totalSize, MEM_RESERVE, PAGE_READWRITE);

   // allocate
   VirtualAlloc(allocPtr, committedSize, MEM_COMMIT, PAGE_READWRITE);

   return (pos_t)allocPtr;
}

void SystemRoutineProvider :: Exit(pos_t exitCode)
{
   ::ExitProcess(exitCode);
}

void SystemRoutineProvider :: CloseThreadHandle(TLSEntry* entry, bool withExit, pos_t exitCode)
{
   ::CloseHandle(entry->tls_sync_event);

   if (withExit)
      ::ExitThread(exitCode);
}