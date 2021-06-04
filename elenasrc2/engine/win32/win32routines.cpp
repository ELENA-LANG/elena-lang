//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Win32 ELENA System Routines
//
//                                              (C)2018-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include <windows.h>

using namespace _ELENA_;

static uintptr_t CriticalHandler = 0; 

#define CALL_FIRST 1  

void SystemRoutineProvider :: RaiseError(int code)
{
   ::RaiseException(code, 0, 0, 0);
}

intptr_t SystemRoutineProvider::AlignHeapSize(intptr_t size)
{
   return align(size, 128);
}

LONG WINAPI ELENAVectoredHandler(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
   int r = 0;
   switch (ExceptionInfo->ExceptionRecord->ExceptionCode)
   {
      case EXCEPTION_BREAKPOINT:
      case EXCEPTION_SINGLE_STEP:
      case EXCEPTION_NONCONTINUABLE_EXCEPTION:
      case CONTROL_C_EXIT:
         return EXCEPTION_CONTINUE_SEARCH;
      case EXCEPTION_ACCESS_VIOLATION:
         ExceptionInfo->ContextRecord->Edx = ExceptionInfo->ContextRecord->Eip;
         ExceptionInfo->ContextRecord->Eax = ELENA_ERR_ACCESS_VIOLATION;
         ExceptionInfo->ContextRecord->Eip = CriticalHandler;

         return EXCEPTION_CONTINUE_EXECUTION;
      case ELENA_ERR_OUT_OF_MEMORY:
         ExceptionInfo->ContextRecord->Edx = ExceptionInfo->ContextRecord->Eip;
         ExceptionInfo->ContextRecord->Eax = ELENA_ERR_OUT_OF_MEMORY;
         ExceptionInfo->ContextRecord->Eip = CriticalHandler;
         ExceptionInfo->ContextRecord->Ebx = 0;

         return EXCEPTION_CONTINUE_EXECUTION;
      case EXCEPTION_INT_DIVIDE_BY_ZERO:
      case EXCEPTION_FLT_DIVIDE_BY_ZERO:
         ExceptionInfo->ContextRecord->Edx = ExceptionInfo->ContextRecord->Eip;
         ExceptionInfo->ContextRecord->Eax = ELENA_ERR_DIVIDE_BY_ZERO;
         ExceptionInfo->ContextRecord->Eip = CriticalHandler;

         return EXCEPTION_CONTINUE_EXECUTION;
      default:
         ExceptionInfo->ContextRecord->Edx = ExceptionInfo->ContextRecord->Eip;
         ExceptionInfo->ContextRecord->Eax = ELENA_ERR_CRITICAL;
         ExceptionInfo->ContextRecord->Eip = CriticalHandler;

         return EXCEPTION_CONTINUE_EXECUTION;
   }

   return EXCEPTION_CONTINUE_SEARCH;
}

void SystemRoutineProvider :: InitCriticalStruct(uintptr_t criticalHandler)
{
   CriticalHandler = criticalHandler;

   AddVectoredExceptionHandler(CALL_FIRST, ELENAVectoredHandler);
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

   return !r ? 0 : (uintptr_t)allocPtr;
}

uintptr_t SystemRoutineProvider :: ExpandPerm(void* allocPtr, size_t newSize)
{
   // allocate
   LPVOID r = VirtualAlloc(allocPtr, newSize, MEM_COMMIT, PAGE_READWRITE);

   return !r ? 0 : (uintptr_t)allocPtr;
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
