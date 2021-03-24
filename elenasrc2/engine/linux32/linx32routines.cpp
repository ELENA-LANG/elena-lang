//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Linux32 ELENA System Routines
//
//                                              (C)2020, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include <sys/mman.h>
#include <errno.h>

using namespace _ELENA_;

void SystemRoutineProvider::InitCriticalStruct(uintptr_t criticalHandler)
{
   //pos_t previousHeader = 0;

   //// ; set SEH handler / frame / stack pointers
   //__asm {
   //   mov  eax, header
   //   mov  ecx, fs: [0]
   //   mov  previousHeader, ecx
   //   mov  fs : [0] , eax
   //}

   //header->previousStruct = previousHeader;
   //header->handler = criticalHandler;
}

TLSEntry* SystemRoutineProvider::GetTLSEntry(pos_t tlsIndex)
{
   TLSEntry* entry = nullptr;

   //// ; GCXT: assign tls entry
   //__asm {
   //   mov  ebx, tlsIndex
   //   mov  ecx, fs: [2Ch]
   //   mov  edx, [ecx + ebx * 4]
   //   mov  entry, edx
   //}

   return entry;
}

void SystemRoutineProvider::InitTLSEntry(pos_t threadIndex, pos_t tlsIndex, ProgramHeader* frameHeader, pos_t* threadTable)
{
   //TLSEntry* entry = GetTLSEntry(tlsIndex);

   //entry->tls_flags = 0;
   //entry->tls_sync_event = ::CreateEvent(0, -1, 0, 0);
   //entry->tls_et_current = &frameHeader->root_exception_struct;
   //entry->tls_threadindex = threadIndex;

   //threadTable[threadIndex] = (pos_t)entry;
}

inline void printNumber(int value)
{
      IdentifierString s;
      s.appendInt(value);

      ident_t pstr = s;
      for(int i = 0; i < getlength(s); i++)
         putchar(pstr[i]);
}

pos_t SystemRoutineProvider::NewHeap(int totalSize, int committedSize)
{
   void* allocPtr = mmap(NULL, totalSize, PROT_READ | PROT_WRITE,
      MAP_SHARED | MAP_ANONYMOUS, -1, 0);

   if (allocPtr == (void*)INVALID_REF) {
      /*IdentifierString s;
      s.appendInt(errno);

      ident_t pstr = s;
      for(int i = 0; i < getlength(s); i++)
         putchar(pstr[i]);*/

      ::exit(errno);
   }

   return (pos_t)allocPtr;
}

void SystemRoutineProvider::Exit(pos_t exitCode)
{
   ::exit(exitCode);
}

void SystemRoutineProvider::CloseThreadHandle(TLSEntry* entry, bool withExit, pos_t exitCode)
{
   //::CloseHandle(entry->tls_sync_event);

   //if (withExit)
   //   ::ExitThread(exitCode);
}
