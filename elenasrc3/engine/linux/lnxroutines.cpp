//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  Linux ELENA System Routines
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "core.h"

#include <sys/mman.h>
#include <signal.h>
#include <errno.h>

using namespace elena_lang;

static uintptr_t CriticalHandler = 0;

uintptr_t SystemRoutineProvider::NewHeap(int totalSize, int committedSize)
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

   return (uintptr_t)allocPtr;
}

void SystemRoutineProvider :: Exit(int exitCode)
{
   ::exit(exitCode);
}

#if __i386__

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

#elif __x86_64__

static void ELENASignalHandler(int sig, siginfo_t* si, void* unused)
{
   ucontext_t* u = (ucontext_t*)unused;

   switch (sig) {
   case SIGFPE:
      u->uc_mcontext.gregs[REG_RDX] = u->uc_mcontext.gregs[REG_RIP];
      u->uc_mcontext.gregs[REG_RAX] = ELENA_ERR_DIVIDE_BY_ZERO;
      u->uc_mcontext.gregs[REG_RIP] = CriticalHandler;
      break;
   case SIGSEGV:
      u->uc_mcontext.gregs[REG_RDX] = u->uc_mcontext.gregs[REG_RIP];
      u->uc_mcontext.gregs[REG_RAX] = ELENA_ERR_ACCESS_VIOLATION;
      u->uc_mcontext.gregs[REG_RIP] = CriticalHandler;
      break;
   default:
      u->uc_mcontext.gregs[REG_RDX] = u->uc_mcontext.gregs[REG_RIP];
      u->uc_mcontext.gregs[REG_RAX] = ELENA_ERR_CRITICAL;
      u->uc_mcontext.gregs[REG_RIP] = CriticalHandler;
      break;
   }
}


#elif __aarch64__

static void ELENASignalHandler(int sig, siginfo_t* si, void* unused)
{
   ucontext_t* u = (ucontext_t*)unused;

   switch (sig) {
      case SIGFPE:
         u->uc_mcontext.regs[9] = u->uc_mcontext.pc;
         u->uc_mcontext.regs[0] = ELENA_ERR_DIVIDE_BY_ZERO;
         u->uc_mcontext.pc = CriticalHandler;
         break;
      case SIGSEGV:
         u->uc_mcontext.regs[9] = u->uc_mcontext.pc;
         u->uc_mcontext.regs[0] = ELENA_ERR_ACCESS_VIOLATION;
         u->uc_mcontext.pc = CriticalHandler;
         break;
      default:
         u->uc_mcontext.regs[9] = u->uc_mcontext.pc;
         u->uc_mcontext.regs[0] = ELENA_ERR_CRITICAL;
         u->uc_mcontext.pc = CriticalHandler;
         break;
   }
}

#elif __PPC64__

static void ELENASignalHandler(int sig, siginfo_t* si, void* unused)
{
   ucontext_t* u = (ucontext_t*)unused;

   switch (sig) {
      case SIGFPE:
         u->uc_mcontext.gp_regs.r9 = u->uc_mcontext.gp_regs.pc;
         u->uc_mcontext.gp_regs.r0 = ELENA_ERR_DIVIDE_BY_ZERO;
         u->uc_mcontext.gp_regs.pc = CriticalHandler;
         break;
      case SIGSEGV:
         u->uc_mcontext.gp_regs.r9 = u->uc_mcontext.gp_regs.pc;
         u->uc_mcontext.gp_regs.r0 = ELENA_ERR_ACCESS_VIOLATION;
         u->uc_mcontext.gp_regs.pc = CriticalHandler;
         break;
      default:
         u->uc_mcontext.gp_regs.r9 = u->uc_mcontext.gp_regs.pc;
         u->uc_mcontext.gp_regs.r0 = ELENA_ERR_CRITICAL;
         u->uc_mcontext.gp_regs.pc = CriticalHandler;
         break;
   }
}

#endif

void SystemRoutineProvider :: InitCriticalStruct(uintptr_t criticalHandler)
{
   CriticalHandler = criticalHandler;

   struct sigaction sa;
   sa.sa_flags = SA_SIGINFO;
   sigemptyset(&sa.sa_mask);
   sa.sa_sigaction = ELENASignalHandler;
   if (sigaction(SIGSEGV, &sa, NULL) == -1)
      throw InternalError(errAborted);
}