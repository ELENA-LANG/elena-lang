//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Linux Debugger adapter class implementation
//                                             (C)2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtkliinuix/lnxdebugadapter.h"

using namespace elena_lang;

// --- BreakpointContext ---

BreakpointContext :: BreakpointContext()
{
}

// --- ThreadContext ---

ThreadContext :: ThreadContext(/*Debugger* debugger, pid_t pid*/)
{
/*   this->threadId = pid;

   this->state = NULL;
   this->atCheckPoint = false;

   this->debugger = debugger;*/
}

// --- ProcessException ---

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

// --- DebugProcess ---

DebugProcess :: DebugProcess()
   : threads(nullptr, freeobj)
{
   currentId = traceeId = 0;
   current = nullptr;

   started = false;
}

bool DebugProcess :: startProcess(const char* exePath, const char* cmdLine, const char* appPath)
{
   const char* path_str = path.str();
   const char* cmdLine_str = cmdLine.str();

   traceeId = fork();
   if (traceeId >= 0) {  /* fork succeeded */
      if (traceeId == 0) { /* fork() returns 0 for the child process */
         ptrace(PTRACE_TRACEME, 0, NULL, NULL);

         //execlp("gnome-terminal", "gnome-terminal", "-x", exePath, cmdLine, NULL );

         char* const* args = (char* const*)generateArguments(cmdLine_str);
         int retVal = execv(path_str, args);

//         const char* exeName = exePath + path_t(exePath).findLast(PATH_SEPARATOR) + 1;
//
//         execl(exePath, exeName, cmdLine, 0);
      }
      else { /* parent process */
         started = true;
         exception.code = 0;

         // enabling multi-threading debugging
         ptrace(PTRACE_SETOPTIONS, traceeId, NULL, PTRACE_O_TRACECLONE/* | PTRACE_O_TRACEFORK*/);

         current = new ThreadContext(this, traceeId);
         threads.add(traceeId, current);

         breakpoints.setSoftwareBreakpoints(current);
      }
   }
   else return false;

   return true;
}

void DebugProcess :: processEvent()
{
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
   _flag &= ~(1 << eventId);
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
      if (err == ETIMEDOUT) {
         event = 0;
         break;
      }
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

// --- LnxDebugAdapter ---

void* debugEventThread(void* controller)
{
   ((DebugControllerBase*)controller)->debugThread();

   return nullptr;
}

LnxDebugAdapter :: LnxDebugAdapter()
//   : _debugProcess(CONSOLE_OUTPUT_TEXT), _threadId(NULL)
{
}

bool LnxDebugAdapter :: startThread(DebugControllerBase* controller)
{
   int err = pthread_create(&threadId, nullptr, &debugEventThread, controller);

   if (err != 0) {
      return false;
   }

   return true;
}

void LnxDebugAdapter :: resetException()
{
/*   _exception.address = 0;
   _exception.code = 0;

   _debugProcess.resetException();*/
}

bool LnxDebugAdapter :: startProgram(path_t exePath, path_t cmdLine, path_t appPath, StartUpSettings&)
{
   if (_debugProcess.startProcess(exePath.str(), cmdLine.str(), appPath.str())
   {
      _debugProcess.processEvent(/*INFINITE*/);

      return true;
   }
   return false;
}
