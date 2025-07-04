//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Win32 ELENA System Routines
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "core.h"
#include "windows/pehelper.h"

#include <windows.h>

using namespace elena_lang;

static uintptr_t CriticalHandler = 0;

#define CALL_FIRST 1  

void* SystemRoutineProvider::RetrieveMDataPtr(void* imageBase, pos_t imageLength)
{
   ImageSection header(imageBase, imageLength);
   MemoryReader reader(&header);
   addr_t addr = 0;
   if (PEHelper::seekSection(reader, ".mdata", addr)) {
      return (void*)addr;
   }

   return nullptr;
}

size_t SystemRoutineProvider :: AlignHeapSize(size_t size)
{
   return alignSize(size, 128);
}

uintptr_t SystemRoutineProvider :: NewHeap(size_t totalSize, size_t committedSize)
{
   // reserve
   void* allocPtr = VirtualAlloc(nullptr, totalSize, MEM_RESERVE, PAGE_READWRITE);

   // allocate
   VirtualAlloc(allocPtr, committedSize, MEM_COMMIT, PAGE_READWRITE);

   return (uintptr_t)allocPtr;
}

uintptr_t SystemRoutineProvider :: ExpandHeap(void* allocPtr, size_t newSize)
{
   // allocate
   LPVOID r = VirtualAlloc(allocPtr, newSize, MEM_COMMIT, PAGE_READWRITE);

   assert(r == allocPtr);

   return !r ? 0 : (uintptr_t)allocPtr;
}

uintptr_t SystemRoutineProvider :: ExpandPerm(void* allocPtr, size_t newSize)
{
   // allocate
   LPVOID r = VirtualAlloc(allocPtr, newSize, MEM_COMMIT, PAGE_READWRITE);

   assert(r == allocPtr);

   return !r ? 0 : (uintptr_t)allocPtr;
}

void* SystemRoutineProvider :: CreateThread(size_t tt_index, int stackSize, int flags, void* threadProc)
{
   return ::CreateThread(
      nullptr,                            // default security attributes
      stackSize,                          // use default stack size  
      (LPTHREAD_START_ROUTINE)threadProc, // thread function name
      (LPVOID)tt_index,                   // argument to thread function 
      flags,                              // use default creation flags 
      nullptr);
}

void SystemRoutineProvider :: RaiseError(int code)
{
   ::RaiseException(code, 0, 0, 0);
}

void SystemRoutineProvider :: Exit(int exitCode)
{
   ::ExitProcess(exitCode);
}

void SystemRoutineProvider :: ExitThread(int exitCode)
{
   ::ExitThread(exitCode);
}

#if _M_IX86 || __i386__

LONG WINAPI ELENAVectoredHandler(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
   int r = 0;
   switch (ExceptionInfo->ExceptionRecord->ExceptionCode) {
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
      case STATUS_STACK_OVERFLOW:
         ExceptionInfo->ContextRecord->Edx = ExceptionInfo->ContextRecord->Eip;
         ExceptionInfo->ContextRecord->Eax = ELENA_ERR_STACKOVERFLOW;
         ExceptionInfo->ContextRecord->Eip = CriticalHandler;
         return EXCEPTION_CONTINUE_EXECUTION;
      case 0x000006ba:
         // !! HOTFIX : temporally ignore
         return EXCEPTION_CONTINUE_SEARCH;
      default:
         if (ExceptionInfo->ExceptionRecord->ExceptionCode < 0xE0000000) {
            ExceptionInfo->ContextRecord->Edx = ExceptionInfo->ContextRecord->Eip;
            ExceptionInfo->ContextRecord->Eax = ELENA_ERR_CRITICAL;
            ExceptionInfo->ContextRecord->Eip = CriticalHandler;

            return EXCEPTION_CONTINUE_EXECUTION;
         }
         else return EXCEPTION_CONTINUE_SEARCH;
   }

   return EXCEPTION_CONTINUE_SEARCH;
}

#elif _M_X64

LONG WINAPI ELENAVectoredHandler(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
   switch (ExceptionInfo->ExceptionRecord->ExceptionCode) {
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
      case 0x000006ba:
         // !! HOTFIX : temporally ignore
         return EXCEPTION_CONTINUE_SEARCH;
      default:
         ExceptionInfo->ContextRecord->Rdx = ExceptionInfo->ContextRecord->Rip;
         ExceptionInfo->ContextRecord->Rax = ELENA_ERR_CRITICAL;
         ExceptionInfo->ContextRecord->Rip = CriticalHandler;

         return EXCEPTION_CONTINUE_EXECUTION;
   }

   return EXCEPTION_CONTINUE_SEARCH;
}

#endif

void SystemRoutineProvider :: InitCriticalStruct(uintptr_t criticalHandler)
{
   CriticalHandler = criticalHandler;

   AddVectoredExceptionHandler(CALL_FIRST, ELENAVectoredHandler);
}

long long SystemRoutineProvider :: GenerateSeed()
{
   SYSTEMTIME st;
   FILETIME ft;

   GetSystemTime(&st);
   SystemTimeToFileTime(&st, &ft);

   long long seed = ft.dwHighDateTime;
   seed <<= 32;
   seed |= ft.dwLowDateTime;

   return seed;
}

void SystemRoutineProvider :: InitMTASignals(SystemEnv* env, size_t index)
{
   env->th_table->slots[index].content->tt_sync_event = ::CreateEvent(0, -1, 0, 0);
   env->th_table->slots[index].content->tt_flags = 0;
}

void SystemRoutineProvider :: ClearMTASignals(SystemEnv* env, size_t index)
{
   ::CloseHandle(env->th_table->slots[index].content->tt_sync_event);

   env->th_table->slots[index].content->tt_sync_event = nullptr;
   env->th_table->slots[index].content->tt_flags = 0;
}

void SystemRoutineProvider :: GCSignalStop(void* handle)
{
   ::SetEvent((HANDLE)handle);
}

void SystemRoutineProvider :: GCWaitForSignals(size_t count, void* handles)
{
   if (count != 0)
      ::WaitForMultipleObjects((DWORD)count, (HANDLE*)handles, -1, -1);
}

void SystemRoutineProvider :: GCWaitForSignal(void* handle)
{
   ::WaitForSingleObject((HANDLE)handle, -1);
}

void SystemRoutineProvider :: GCSignalClear(void* handle)
{
   ::ResetEvent((HANDLE)handle);
}
