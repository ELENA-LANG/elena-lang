//----------------------------------------------------------------------r-----
//		E L E N A   P r o j e c t:  Win64 ELENA System Routines
//
//                                              (C)2020-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include <windows.h>

using namespace _ELENA_;

static uintptr_t CriticalHandler = 0;

#define CALL_FIRST 1  

void SystemRoutineProvider::RaiseError(int code)
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
         ExceptionInfo->ContextRecord->Rdx = ExceptionInfo->ContextRecord->Rip;
         ExceptionInfo->ContextRecord->Rax = ELENA_ERR_ACCESS_VIOLATION;
         ExceptionInfo->ContextRecord->Rip = CriticalHandler;

         return EXCEPTION_CONTINUE_EXECUTION;
      case ELENA_ERR_OUT_OF_MEMORY:
         ExceptionInfo->ContextRecord->Rdx = ExceptionInfo->ContextRecord->Rip;
         ExceptionInfo->ContextRecord->Rax = ELENA_ERR_OUT_OF_MEMORY;
         ExceptionInfo->ContextRecord->Rip = CriticalHandler;
         ExceptionInfo->ContextRecord->Rbx = 0;

         return EXCEPTION_CONTINUE_EXECUTION;
      case EXCEPTION_INT_DIVIDE_BY_ZERO:
      case EXCEPTION_FLT_DIVIDE_BY_ZERO:
         ExceptionInfo->ContextRecord->Rdx = ExceptionInfo->ContextRecord->Rip;
         ExceptionInfo->ContextRecord->Rax = ELENA_ERR_DIVIDE_BY_ZERO;
         ExceptionInfo->ContextRecord->Rip = CriticalHandler;

         return EXCEPTION_CONTINUE_EXECUTION;
      default:
         ExceptionInfo->ContextRecord->Rdx = ExceptionInfo->ContextRecord->Rip;
         ExceptionInfo->ContextRecord->Rax = ELENA_ERR_CRITICAL;
         ExceptionInfo->ContextRecord->Rip = CriticalHandler;

         return EXCEPTION_CONTINUE_EXECUTION;
   }

   return EXCEPTION_CONTINUE_SEARCH;
}

void SystemRoutineProvider::InitCriticalStruct(uintptr_t criticalHandler)
{
   CriticalHandler = criticalHandler;

   AddVectoredExceptionHandler(CALL_FIRST, ELENAVectoredHandler);
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

uintptr_t SystemRoutineProvider::ExpandHeap(void* allocPtr, int newSize)
{
   // allocate
   LPVOID r = VirtualAlloc(allocPtr, newSize, MEM_COMMIT, PAGE_READWRITE);

   return (uintptr_t)allocPtr;
}

uintptr_t SystemRoutineProvider::ExpandPerm(void* allocPtr, size_t newSize)
{
   // allocate
   LPVOID r = VirtualAlloc(allocPtr, newSize, MEM_COMMIT, PAGE_READWRITE);

   return !r ? 0 : (uintptr_t)allocPtr;
}
