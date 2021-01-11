//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Win32 ELENA System Routines
//
//                                              (C)2018-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include <windows.h>
#include <errhandlingapi.h>

using namespace _ELENA_;

void SystemRoutineProvider :: InitCriticalStruct(CriticalStruct* header, uintptr_t criticalHandler)
{
   uintptr_t previousHeader = 0;

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

   threadTable[threadIndex] = (uintptr_t)entry;
}

uintptr_t SystemRoutineProvider :: NewHeap(int totalSize, int committedSize)
{
   // reserve
   void* allocPtr = VirtualAlloc(nullptr, totalSize, MEM_RESERVE, PAGE_READWRITE);

   // allocate
   VirtualAlloc(allocPtr, committedSize, MEM_COMMIT, PAGE_READWRITE);

   return (uintptr_t)allocPtr;
}

uintptr_t SystemRoutineProvider :: ExpandHeap(void* allocPtr, int newSize)
{
   // allocate
   LPVOID r = VirtualAlloc(allocPtr, newSize, MEM_COMMIT, PAGE_READWRITE);

   return (uintptr_t)allocPtr;
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

void SystemRoutineProvider :: GCSignalStop(int handle)
{
   ::SetEvent((HANDLE)handle);
}

void SystemRoutineProvider :: GCSignalClear(int handle)
{
   ::ResetEvent((HANDLE)handle);
}

void SystemRoutineProvider :: GCWaitForSignal(int handle)
{
   ::WaitForSingleObject((HANDLE)handle, -1);
}

void SystemRoutineProvider :: GCWaitForSignals(int count, int* handles)
{
   if (count > 0)
      ::WaitForMultipleObjects(count, (HANDLE*)handles, -1, -1);
}
