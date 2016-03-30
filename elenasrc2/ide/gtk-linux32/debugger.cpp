//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the Debugger class and its helpers implementation
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "debugger.h"
#include "../idecommon.h"

#include <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <stddef.h>

using namespace _ELENA_;

// --- main thread that is the debugger residing over a debuggee ---

void* debugEventThread(void* controller)
{
   ((_DebugController*)controller)->debugThread();

   return NULL;
}

// --- DebugEventManager ---

void DebugEventManager :: init()
{
   _flag = 1 << DEBUG_ACTIVE;
   pthread_mutex_init(&_lock, NULL);
   pthread_cond_init(&_event, NULL);
}

void DebugEventManager :: resetEvent(int eventId)
{
   pthread_mutex_lock(&_lock);
   _flag &= ~(_flag = 1 << eventId);
   pthread_mutex_unlock(&_lock);
}

void DebugEventManager :: setEvent(int eventId)
{
   pthread_mutex_lock(&_lock);

   _flag |= (1 << eventId);
   pthread_cond_signal(&_event);

   pthread_mutex_unlock(&_lock);
}

int DebugEventManager :: waitForAnyEvent()
{
   int retVal = 0;

   pthread_mutex_lock(&_lock);
   while (_flag == 0)
      pthread_cond_wait(&_event, &_lock);

   for (int i = 0 ; i < MAX_DEBUG_EVENT ; i++) {
      int mask = 1 << i;
      if ((_flag & mask)==mask) {
         retVal = i;
         break;
      }
   }
   pthread_mutex_unlock(&_lock);

   return retVal;
}

bool DebugEventManager :: waitForEvent(int event, int timeout)
{
   timespec to;

   pthread_mutex_lock(&_lock);
   to.tv_sec = time(NULL) + timeout;
   to.tv_nsec = 0;

   int err;
   int mask = 1 << event;
   while ((_flag & mask) == 0) {
      err = pthread_cond_timedwait(&_event, &_lock, &to);
      if (err == ETIMEDOUT)
         return 0;
   }
   pthread_mutex_unlock(&_lock);

   return event;
}

void DebugEventManager :: close()
{
   _flag = 0;
   pthread_cond_destroy(&_event);
   pthread_mutex_destroy(&_lock);
}

// --- ProcessException---

const char* ProcessException :: Text()
{
   switch (code) {
      case SIGSEGV:
         return ACCESS_VIOLATION_EXCEPTION_TEXT;
//      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
//         return ARRAY_BOUNDS_EXCEEDED_EXCEPTION_TEXT;
//      case EXCEPTION_DATATYPE_MISALIGNMENT:
//         return DATATYPE_MISALIGNMENT_EXCEPTION_TEXT;
//      case EXCEPTION_FLT_DENORMAL_OPERAND:
//         return FLT_DENORMAL_OPERAND_EXCEPTION_TEXT;
//      case EXCEPTION_FLT_DIVIDE_BY_ZERO:
//         return FLT_DIVIDE_BY_ZERO_EXCEPTION_TEXT;
//      case EXCEPTION_FLT_INEXACT_RESULT:
//         return FLT_INEXACT_RESULT_EXCEPTION_TEXT;
//      case EXCEPTION_FLT_INVALID_OPERATION:
//         return FLT_INVALID_OPERATION_EXCEPTION_TEXT;
//      case EXCEPTION_FLT_OVERFLOW:
//         return FLT_OVERFLOW_EXCEPTION_TEXT;
//      case EXCEPTION_FLT_STACK_CHECK:
//         return FLT_STACK_CHECK_EXCEPTION_TEXT;
//      case EXCEPTION_FLT_UNDERFLOW:
//         return FLT_UNDERFLOW_EXCEPTION_TEXT;
//      case EXCEPTION_ILLEGAL_INSTRUCTION:
//         return ILLEGAL_INSTRUCTION_EXCEPTION_TEXT;
//      case EXCEPTION_IN_PAGE_ERROR:
//         return PAGE_ERROR_EXCEPTION_TEXT;
//      case EXCEPTION_INT_DIVIDE_BY_ZERO:
//         return INT_DIVIDE_BY_ZERO_EXCEPTION_TEXT;
//      case EXCEPTION_INT_OVERFLOW:
//         return INT_OVERFLOW_EXCEPTION_TEXT;
//      case EXCEPTION_INVALID_DISPOSITION:
//         return INVALID_DISPOSITION_EXCEPTION_TEXT;
//      case EXCEPTION_NONCONTINUABLE_EXCEPTION:
//         return NONCONTINUABLE_EXCEPTION_EXCEPTION_TEXT;
//      case EXCEPTION_PRIV_INSTRUCTION:
//         return PRIV_INSTRUCTION_EXCEPTION_TEXT;
//      case EXCEPTION_STACK_OVERFLOW:
//         return STACK_OVERFLOW_EXCEPTION_TEXT;
//      case ELENA_ERR_OUTOF_MEMORY:
//         return GC_OUTOF_MEMORY_EXCEPTION_TEXT;
	  default:
         return UNKNOWN_EXCEPTION_TEXT;
   }
}

// --- ThreadContext ---

ThreadContext :: ThreadContext(Debugger* debugger, pid_t pid)
{
   this->threadId = pid;

   this->state = NULL;
   this->atCheckPoint = false;

   this->debugger = debugger;
}

void ThreadContext :: refresh()
{
   ptrace(PTRACE_GETREGS, threadId, NULL, &context);
//   if (context.SegFs==0) {                                 // !! hotfix
//      context.SegFs=0x38;
//      SetThreadContext(hThread, &context);
//   }
}

void ThreadContext :: setCheckPoint()
{
   atCheckPoint = true;
}


void ThreadContext :: set_breakpoint_addr(void *addr, int n)
{
   int ret;

   // !! temporal
   if (n == 0) {
      ret = ptrace(PTRACE_POKEUSER, threadId,
                     offsetof(struct user, u_debugreg[0]), addr);
   }
   else if (n == 7) {
      ret = ptrace(PTRACE_POKEUSER, threadId,
                     offsetof(struct user, u_debugreg[7]), addr);
   }

   //ret = ptrace(PTRACE_POKEUSER, threadId,
   //               offsetof(struct user, u_debugreg[n]), addr);
 }

void ThreadContext :: setHardwareBreakpoint(size_t breakpoint)
{
   set_breakpoint_addr((void*)breakpoint, 0);
   set_breakpoint_addr((void*)1, 7);
//   context.Dr0 = breakpoint;
//   context.Dr7 = 0x000001;
   this->breakpoint.hardware = true;
}

void ThreadContext :: clearHardwareBreakpoint()
{
   set_breakpoint_addr(NULL, 0);
   set_breakpoint_addr(NULL, 7);
//   context.Dr0 = 0x0;
//   context.Dr7 = 0x0;
   breakpoint.hardware = false;
}

void ThreadContext :: clearSoftwareBreakpoint(size_t breakpoint, char substitute)
{
   writeDump(breakpoint, &substitute, 1);
}

unsigned char ThreadContext :: setSoftwareBreakpoint(size_t breakpoint)
{
   unsigned char code = 0;
   unsigned char terminator = 0xCC;

   readDump(breakpoint, (char*)&code, 1);
   writeDump(breakpoint, (char*)&terminator, 1);

   return code;
}

void ThreadContext :: readDump(size_t address, char* dump, size_t length)
{
   int index = 0;
   long val;
   while (length > 0) {
      val = ptrace(PTRACE_PEEKDATA,
                          threadId, address + index * 4,
                          NULL);

      if (length > 3) {
         memcpy(dump + index * 4, &val, 4);
         length -= 4;
      }
      else {
         memcpy(dump + index * 4, &val, length);
         length = 0;
      }

      index++;
   }
}

void ThreadContext :: writeDump(size_t address, char* dump, size_t length)
{
   int index = 0;
   long val;
   while (length > 0) {
      if (length > 3) {
         memcpy(&val, dump + index * 4, 4);
         length -= 4;
      }
      else {
         val = ptrace(PTRACE_PEEKDATA,
                          threadId, address + index * 4,
                          NULL);

         memcpy(&val, dump + index * 4, length);
         length = 0;
      }

      ptrace(PTRACE_POKEDATA,
                          threadId, address + index * 4,
                          (void*)val);

      index++;
   }
}

size_t ThreadContext :: ClassVMT(size_t objectPtr)
{
   long val = ptrace(PTRACE_PEEKDATA,
                    threadId, (void*)(objectPtr - 4),
                    NULL);

   return val;
}

size_t ThreadContext :: VMTFlags(size_t vmtPtr)
{
   long val = ptrace(PTRACE_PEEKDATA,
                    threadId, (void*)(vmtPtr - 8),
                    NULL);

   return val;
}

size_t ThreadContext :: ObjectPtr(size_t address)
{
   long val = ptrace(PTRACE_PEEKDATA,
                    threadId, (void*)(address),
                    NULL);

   return val;
}

void ThreadContext :: setEIP(size_t address)
{
   ptrace(PTRACE_POKEUSER, threadId,
                  offsetof(struct user, regs.eip), address);
}

void ThreadContext :: setTrapFlag()
{
   debugger->setStepMode();

//   context.ContextFlags = CONTEXT_CONTROL;
//   context.EFlags |= 0x100;
//   SetThreadContext(hThread, &context);
}

void ThreadContext :: resetTrapFlag()
{
   debugger->resetStepMode();

//   context.ContextFlags = CONTEXT_CONTROL;
//   context.EFlags &= ~0x100;
//   SetThreadContext(hThread, &context);
}


// --- BreakpointContext ---

BreakpointContext :: BreakpointContext()
{
}

void BreakpointContext :: addBreakpoint(size_t address, ThreadContext* context, bool started)
{
   if (started) {
      breakpoints.add(address, context->setSoftwareBreakpoint(address));
   }
   else breakpoints.add(address, 0);
}

void BreakpointContext :: removeBreakpoint(size_t address, ThreadContext* context, bool started)
{
   if (started) {
      context->clearSoftwareBreakpoint(address, breakpoints.get(address));
      if (context->breakpoint.software && context->breakpoint.next==address) {

         context->breakpoint.clearSoftware();
         context->resetTrapFlag();
      }
   }
   breakpoints.erase(address);
}

void BreakpointContext :: setSoftwareBreakpoints(ThreadContext* context)
{
   Map<size_t, char>::Iterator breakpoint = breakpoints.start();
   while (!breakpoint.Eof()) {
      *breakpoint = context->setSoftwareBreakpoint(breakpoint.key());

      breakpoint++;
   }
}

void BreakpointContext :: setHardwareBreakpoint(size_t address, ThreadContext* context, bool withStackControl)
{
   if (address==context->context.eip) {
      context->setTrapFlag();
      context->breakpoint.next = address;
   }
   else context->setHardwareBreakpoint(address);

   if (withStackControl) {
      context->breakpoint.stackLevel = context->context.ebp;
   }
   else context->breakpoint.stackLevel = 0;
}

bool BreakpointContext :: processStep(ThreadContext* context, bool stepMode)
{
   ThreadBreakpoint breakpoint = context->breakpoint;

   if (breakpoint.next != 0) {
      if (breakpoint.software) {
         context->setSoftwareBreakpoint(breakpoint.next);
         context->breakpoint.software = false;
      }
      else context->setHardwareBreakpoint(breakpoint.next);
      context->breakpoint.next = 0;
      if (stepMode)
         context->setTrapFlag();

      return true;
   }

   if (breakpoint.hardware) {
      context->clearHardwareBreakpoint();

      // check stack level to skip recursive entries
      if (context->context.ebp < breakpoint.stackLevel) {
         context->breakpoint.next = context->context.eip;
         context->setTrapFlag();
         return true;
      }
   }

   return false;
}

bool BreakpointContext :: processBreakpoint(ThreadContext* context)
{
   if (breakpoints.exist(context->context.eip - 1)) {
      context->breakpoint.next = context->context.eip - 1;

      context->setEIP(context->breakpoint.next);
      char substitute = breakpoints.get(context->breakpoint.next);
      context->writeDump(context->breakpoint.next, &substitute, 1);

      context->breakpoint.software = true;

      return true;
   }
   else return false;
}

void BreakpointContext :: clear()
{
   breakpoints.clear();
}

// --- Debugger ---

Debugger :: Debugger()
   : threads(NULL, freeobj)
{
   started = false;
   threadId = 0;
//   vmhookAddress = 0;
   current = NULL;

   currentId = traceeId = 0;
   exitCheckPoint = false;
}

bool Debugger :: startProcess(const char* exePath, const char* cmdLine)
{
   traceeId = fork();
   if (traceeId >= 0) {  /* fork succeeded */
      if (traceeId == 0) { /* fork() returns 0 for the child process */
         ptrace(PTRACE_TRACEME, 0, NULL, NULL);

         const char* exeName = exePath + StringHelper::findLast(exePath, PATH_SEPARATOR) + 1;

         execl(exePath, exeName, cmdLine, 0);
      }
      else { /* parent process */
         started = true;
         exception.code = 0;

         // enabling multi-threading debugging
         ptrace(PTRACE_SETOPTIONS, traceeId, NULL, PTRACE_O_TRACECLONE);

         current = new ThreadContext(this, traceeId);
         threads.add(traceeId, current);

         breakpoints.setSoftwareBreakpoints(current);
      }
   }
   else return false;

   return true;
}

void Debugger :: processEvent()
{
   trapped = false;

   int status;
   currentId = waitpid(-1, &status, __WALL);
   if (currentId == -1)
      return;

   // new thread
   if(WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
      if(((status >> 16) & 0xffff) == PTRACE_EVENT_CLONE) {
         pid_t newThreadId;
         if(ptrace(PTRACE_GETEVENTMSG, currentId, 0, &newThreadId) != -1) {
            current = new ThreadContext(this, newThreadId);
            current->refresh();

            threads.add(newThreadId, current);
         }
      }
   }

   // thread closed / killed
   if (WIFEXITED(status) || WIFSIGNALED(status)) {
      current = threads.get(currentId);

      threads.erase(currentId);

      // process closed
      if (threads.Count() == 0) {
         if (current) {
            current->refresh();
            exitCheckPoint = proceedCheckPoint();
         }

         started = false;
      }

      current = NULL;
   }
   else if (WIFSTOPPED(status)) {
      current = threads.get(currentId);

      int stopCode = WSTOPSIG(status);
      processSignal(stopCode);
   }
}

void Debugger :: processSignal(int signal)
{
   if(signal == SIGTRAP && current) {
      if (stepMode) {
         if (breakpoints.processStep(current, stepMode))
            return;

         if (current->context.eip >= minAddress && current->context.eip <= maxAddress) {
            processStep();
         }
         if (!trapped)
            current->setTrapFlag();
      }
      else {
//         // init vm hook address if enabled
//         if (vmhookAddress == -1) {
//            vmhookAddress = current->context.Eip;
//         }

         if (breakpoints.processBreakpoint(current)) {
            current->state = steps.get(current->context.eip);
            trapped = true;
            stepMode = false;
            current->setTrapFlag();
         }
         //else if (vmhookAddress == current->context.Eip) {
         //   trapped = true;
         //}
      }
      current->refresh();
   }
   else if (signal == SIGSEGV) {
      struct __ptrace_peeksiginfo_args mask;
      siginfo_t info;

      mask.nr = 1;
      mask.flags = 0;
      mask.off = 0;

      ptrace(PTRACE_PEEKSIGINFO, currentId, &mask, &info);

      this->exception.code = signal;
      this->exception.address = (int)info.si_addr;
   }
}

void Debugger :: processStep()
{
   current->state = steps.get(current->context.eip);
   if (current->state != NULL) {
      trapped = true;
      stepMode = false;
      proceedCheckPoint();
   }
}

bool Debugger :: proceedCheckPoint()
{
   if (started) {
      if (current->atCheckPoint) {
         current->checkFailed = (current->context.eax == 0);
         current->atCheckPoint = false;
      }
      else current->checkFailed = false;

      return current->checkFailed;
   }
   else return exitCheckPoint;
}

void Debugger :: processVirtualStep(void* state)
{
   current->state = state;
}

void Debugger :: continueProcess()
{
   ptrace(stepMode ? PTRACE_SINGLESTEP : PTRACE_CONT, currentId, NULL, NULL);
}

void Debugger :: addStep(size_t address, void* state)
{
   steps.add(address, state);
   if (address < minAddress)
      minAddress = address;

   if (address > maxAddress)
      maxAddress = address;
}

void Debugger :: setStepMode()
{
//   current->setTrapFlag();
   stepMode = true;
}

void Debugger :: resetStepMode()
{
//   current->setTrapFlag();
   stepMode = false;
}

void Debugger :: setBreakpoint(size_t address, bool withStackLevelControl)
{
   breakpoints.setHardwareBreakpoint(address, current, withStackLevelControl);
}

void Debugger :: setCheckMode()
{
   current->setCheckPoint();
}

bool Debugger :: start(const char* exePath, const char* cmdLine)
{
   if (startProcess(exePath, cmdLine)) {
      processEvent();

      return true;
   }
   else return false;
}

bool Debugger :: proceed(size_t timeout)
{
   processEvent();

   return !trapped;
}

void Debugger :: run()
{
   continueProcess();
}

void Debugger :: stop()
{
   if (!started)
      return;

   kill(traceeId, SIGKILL);

   continueProcess();
}

void Debugger :: addBreakpoint(size_t address)
{
   breakpoints.addBreakpoint(address, current, started);
}

void Debugger :: removeBreakpoint(size_t address)
{
   breakpoints.removeBreakpoint(address, current, started);
}

void Debugger :: clearBreakpoints()
{
   breakpoints.clear();
}

bool Debugger :: startThread(_DebugController* controller)
{
   int err = pthread_create(&threadId, NULL, &debugEventThread, controller);

   if (err != 0) {
      return false;
   }

   return true;
}

void Debugger :: reset()
{
   trapped = false;

   threads.clear();
   current = NULL;

   minAddress = 0xFFFFFFFF;
   maxAddress = 0;
//   vmhookAddress = 0;

   steps.clear();
   breakpoints.clear();

   stepMode = false;
   exitCheckPoint = false;
}

//BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam)
//{
//   if (GetWindowThreadProcessId(hwnd, NULL)==(DWORD)lParam) {
//      _ELENA_::setForegroundWindow(hwnd);
//
//      return FALSE;
//   }
//   else return TRUE;
//}

void Debugger :: activate()
{
//   if (started) {
//      EnumWindows(EnumThreadWndProc, dwCurrentThreadId);
//   }
}

size_t Debugger :: findEntryPoint(const char* programPath)
{
   return 0; // !! temporal
}

bool Debugger :: findSignature(char* signature)
{
   return false; // !! temporal
}

bool Debugger :: initDebugInfo(bool standalone, StreamReader& reader, size_t& debugInfoPtr)
{
   return false; // !! temporal
}
