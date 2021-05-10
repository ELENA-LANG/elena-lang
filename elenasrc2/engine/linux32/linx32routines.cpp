//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Linux32 ELENA System Routines
//
//                                              (C)2020-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include <sys/mman.h>
#include <signal.h>
#include <ucontext.h>
#include <errno.h>

using namespace _ELENA_;

static uintptr_t CriticalHandler = 0;

static void ELENASignalHandler(int sig, siginfo_t* si, void* unused)
{
   ucontext_t* u = (ucontext_t*)unused;

   switch (sig) {
      case SIGFPE:
         u->uc_mcontext.gregs[REG_EDX] = u->uc_mcontext.gregs[REG_EIP];
         u->uc_mcontext.gregs[REG_EAX] = ELENA_ERR_DIVIDE_BY_ZERO;
         u->uc_mcontext.gregs[REG_EIP] = CriticalHandler;
         break;
      case SIGSEGV:
         u->uc_mcontext.gregs[REG_EDX] = u->uc_mcontext.gregs[REG_EIP];
         u->uc_mcontext.gregs[REG_EAX] = ELENA_ERR_ACCESS_VIOLATION;
         u->uc_mcontext.gregs[REG_EIP] = CriticalHandler;
         break;
      default:
         u->uc_mcontext.gregs[REG_EDX] = u->uc_mcontext.gregs[REG_EIP];
         u->uc_mcontext.gregs[REG_EAX] = ELENA_ERR_CRITICAL;
         u->uc_mcontext.gregs[REG_EIP] = CriticalHandler;
         break;
   }
}

void SystemRoutineProvider::InitCriticalStruct(uintptr_t criticalHandler)
{
   CriticalHandler = criticalHandler;

   struct sigaction sa;
   sa.sa_flags = SA_SIGINFO;
   sigemptyset(&sa.sa_mask);
   sa.sa_sigaction = ELENASignalHandler;
   if (sigaction(SIGSEGV, &sa, NULL) == -1)
      throw EAbortException();
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

//inline void printNumber(int value)
//{
//      IdentifierString s;
//      s.appendInt(value);
//
//      ident_t pstr = s;
//      for(int i = 0; i < getlength(s); i++)
//         putchar(pstr[i]);
//}

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
