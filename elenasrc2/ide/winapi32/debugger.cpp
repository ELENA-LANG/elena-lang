//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Debugger class and its helpers implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "debugger.h"
#include "..\idecommon.h"
#include "win32\pehelper.h"
#include "elenamachine.h"

#include "core.h"

using namespace _ELENA_;

// --- main thread that is the debugger residing over a debuggee ---

BOOL WINAPI debugEventThread(_DebugController* controller)
{
   controller->debugThread();

   ExitThread(TRUE);

   return TRUE;
}

// --- DebugEventManager ---

void DebugEventManager :: init()
{
   _events[DEBUG_ACTIVE] = CreateEvent(NULL, TRUE, TRUE, NULL);
   _events[DEBUG_CLOSE] = CreateEvent(NULL, TRUE, FALSE, NULL);
   _events[DEBUG_SUSPEND] = CreateEvent(NULL, TRUE, FALSE, NULL);
   _events[DEBUG_RESUME] = CreateEvent(NULL, TRUE, FALSE, NULL);
}

void DebugEventManager :: resetEvent(int event)
{
   ResetEvent(_events[event]);
}

void DebugEventManager :: setEvent(int event)
{
   SetEvent(_events[event]);
}

int DebugEventManager :: waitForAnyEvent()
{
   return WaitForMultipleObjects (MAX_DEBUG_EVENT, _events, FALSE, INFINITE);
}

bool DebugEventManager :: waitForEvent(int event, int timeout)
{
   return (WaitForSingleObject(_events[event], timeout)==WAIT_OBJECT_0);
}

void DebugEventManager :: close()
{
   for (int i = 0 ; i < MAX_DEBUG_EVENT ; i++) {
      if (_events[i]) {
         CloseHandle(_events[i]);
         _events[i] = NULL;
      }
   }
}

// --- ProcessException---

wchar_t* ProcessException :: Text()
{
   switch (code) {
      case EXCEPTION_ACCESS_VIOLATION:
         return ACCESS_VIOLATION_EXCEPTION_TEXT;
      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
         return ARRAY_BOUNDS_EXCEEDED_EXCEPTION_TEXT;
      case EXCEPTION_DATATYPE_MISALIGNMENT:
         return DATATYPE_MISALIGNMENT_EXCEPTION_TEXT;
      case EXCEPTION_FLT_DENORMAL_OPERAND:
         return FLT_DENORMAL_OPERAND_EXCEPTION_TEXT;
      case EXCEPTION_FLT_DIVIDE_BY_ZERO:
         return FLT_DIVIDE_BY_ZERO_EXCEPTION_TEXT;
      case EXCEPTION_FLT_INEXACT_RESULT:
         return FLT_INEXACT_RESULT_EXCEPTION_TEXT;
      case EXCEPTION_FLT_INVALID_OPERATION:
         return FLT_INVALID_OPERATION_EXCEPTION_TEXT;
      case EXCEPTION_FLT_OVERFLOW:
         return FLT_OVERFLOW_EXCEPTION_TEXT;
      case EXCEPTION_FLT_STACK_CHECK:
         return FLT_STACK_CHECK_EXCEPTION_TEXT;
      case EXCEPTION_FLT_UNDERFLOW:
         return FLT_UNDERFLOW_EXCEPTION_TEXT;
      case EXCEPTION_ILLEGAL_INSTRUCTION:
         return ILLEGAL_INSTRUCTION_EXCEPTION_TEXT;
      case EXCEPTION_IN_PAGE_ERROR:
         return PAGE_ERROR_EXCEPTION_TEXT;
      case EXCEPTION_INT_DIVIDE_BY_ZERO:
         return INT_DIVIDE_BY_ZERO_EXCEPTION_TEXT;
      case EXCEPTION_INT_OVERFLOW:
         return INT_OVERFLOW_EXCEPTION_TEXT;
      case EXCEPTION_INVALID_DISPOSITION:
         return INVALID_DISPOSITION_EXCEPTION_TEXT;
      case EXCEPTION_NONCONTINUABLE_EXCEPTION:
         return NONCONTINUABLE_EXCEPTION_EXCEPTION_TEXT;
      case EXCEPTION_PRIV_INSTRUCTION:
         return PRIV_INSTRUCTION_EXCEPTION_TEXT;
      case EXCEPTION_STACK_OVERFLOW:
         return STACK_OVERFLOW_EXCEPTION_TEXT;
      //case ELENA_ERR_OUTOF_MEMORY:
      //   return GC_OUTOF_MEMORY_EXCEPTION_TEXT;
	  default:
         return UNKNOWN_EXCEPTION_TEXT;
   }
}

// --- ThreadContext ---

ThreadContext :: ThreadContext(HANDLE hProcess, HANDLE hThread)
{
   this->hProcess = hProcess;
   this->hThread = hThread;

   this->state = NULL;
   this->atCheckPoint = false;
   this->autoStep = false;
}

void ThreadContext :: refresh()
{
   context.ContextFlags = CONTEXT_FULL;
   GetThreadContext(hThread, &context);
   if (context.SegFs==0) {                                 // !! hotfix
      context.SegFs=0x38;
      SetThreadContext(hThread, &context);
   }
}

void ThreadContext :: setCheckPoint()
{
   atCheckPoint = true;
}

void ThreadContext :: setTrapFlag()
{
   context.ContextFlags = CONTEXT_CONTROL;
   context.EFlags |= 0x100;
   SetThreadContext(hThread, &context);
}

void ThreadContext :: resetTrapFlag()
{
   context.ContextFlags = CONTEXT_CONTROL;
   context.EFlags &= ~0x100;
   SetThreadContext(hThread, &context);
}

void ThreadContext :: setHardwareBreakpoint(size_t breakpoint)
{
   context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
   context.Dr0 = breakpoint;
   context.Dr7 = 0x000001;
   SetThreadContext(hThread, &context);
   this->breakpoint.hardware = true;
}

void ThreadContext :: clearHardwareBreakpoint()
{
   context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
   context.Dr0 = 0x0;
   context.Dr7 = 0x0;
   SetThreadContext(hThread, &context);
   breakpoint.hardware = false;
}

void ThreadContext :: clearSoftwareBreakpoint(size_t breakpoint, char substitute)
{
   writeDump(breakpoint, &substitute, 1);
}

unsigned char ThreadContext :: setSoftwareBreakpoint(size_t breakpoint)
{
   unsigned char code;
   unsigned char terminator = 0xCC;

   readDump(breakpoint, (char*)&code, 1);
   writeDump(breakpoint, (char*)&terminator, 1);

   return code;
}

bool ThreadContext :: readDump(size_t address, char* dump, size_t length)
{
   unsigned long   size = 0;

   ReadProcessMemory(hProcess, (void*)(address), dump, length, &size);

   return size != 0;
}

void ThreadContext :: writeDump(size_t address, char* dump, size_t length)
{
   unsigned long   size = 0;

   WriteProcessMemory(hProcess, (void*)(address), dump, length, &size);
}

size_t ThreadContext :: ClassVMT(size_t objectPtr)
{
   int             dump = -1;
   unsigned long   size = 0;

   ReadProcessMemory(hProcess, (void*)(objectPtr - elPageVMTOffset32), &dump, 4, &size);

   return dump;
}

size_t ThreadContext :: VMTFlags(size_t vmtPtr)
{
   int             dump = -1;
   unsigned long   size = 0;

   ReadProcessMemory(hProcess, (void*)(vmtPtr - elVMTFlagOffset32), &dump, 4, &size);

   return dump;
}

size_t ThreadContext :: ObjectPtr(size_t address)
{
   int             dump = -1;
   unsigned long   size = 0;

   ReadProcessMemory(hProcess, (void*)(address), &dump, 4, &size);

   return dump;
}

void ThreadContext :: setEIP(size_t address)
{
   context.ContextFlags = CONTEXT_CONTROL;
   GetThreadContext(hThread, &context);
   context.Eip = address;
   SetThreadContext(hThread, &context);
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
   if (address==context->context.Eip) {
      context->setTrapFlag();
      context->breakpoint.next = address;
   }
   else {
      context->setHardwareBreakpoint(address);
   }

   if (withStackControl) {
      context->breakpoint.stackLevel = context->context.Ebp;
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
      if (context->context.Ebp < breakpoint.stackLevel) {
         context->breakpoint.next = context->context.Eip;
         context->setTrapFlag();
         return true;
      }      
   }

   return false;
}

bool BreakpointContext :: processBreakpoint(ThreadContext* context)
{
   if (breakpoints.exist(context->context.Eip - 1)) {
      context->breakpoint.next = context->context.Eip - 1;

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
   init_breakpoint = 0;
   _vmHook = 0;
   threadId = 0;
   current = NULL;
   baseAddress = NULL;

   dwCurrentProcessId = dwCurrentThreadId = 0;
   exitCheckPoint = false;
}

bool Debugger :: startProcess(const wchar_t* exePath, const wchar_t* cmdLine)
{
   PROCESS_INFORMATION pi = { NULL, NULL, 0, 0 };
   STARTUPINFO         si;
   Path				   currentPath;

   currentPath.copySubPath(exePath);

   memset(&si, 0, sizeof(si));

   si.dwFlags = STARTF_USESHOWWINDOW;
   si.wShowWindow = SW_SHOWNORMAL;

   if (!CreateProcess(exePath, (wchar_t*)cmdLine, NULL, NULL, FALSE,
	   CREATE_NEW_CONSOLE | DEBUG_PROCESS, NULL, currentPath, &si, &pi))
   {
      return false;
   }

   if (pi.hProcess)
      CloseHandle(pi.hProcess);

   if (pi.hThread)
      CloseHandle(pi.hThread);

   started = true;
   exception.code = 0;
   needToHandle = false;

   return true;
}

void Debugger :: processEvent(size_t timeout)
{
   DEBUG_EVENT event;

   trapped = false;
   if (WaitForDebugEvent(&event, timeout)) {
      dwCurrentThreadId = event.dwThreadId;
      dwCurrentProcessId = event.dwProcessId;

      switch (event.dwDebugEventCode) {
         case CREATE_PROCESS_DEBUG_EVENT:
            baseAddress = event.u.CreateProcessInfo.lpBaseOfImage;

            current = new ThreadContext(event.u.CreateProcessInfo.hProcess, event.u.CreateProcessInfo.hThread);
            current->refresh();

            threads.add(dwCurrentThreadId, current);

            breakpoints.setSoftwareBreakpoints(current);

            ::CloseHandle(event.u.CreateProcessInfo.hFile);
            break;
         case EXIT_PROCESS_DEBUG_EVENT:
            current = threads.get(dwCurrentThreadId);
            if (current) {
               current->refresh();
               exitCheckPoint = proceedCheckPoint();
            }
            threads.clear();
            current = NULL;
            started = false;
            break;
         case CREATE_THREAD_DEBUG_EVENT:
            current = new ThreadContext((*threads.start())->hProcess, event.u.CreateThread.hThread);
            current->refresh();

            threads.add(dwCurrentThreadId, current);
            break;
         case EXIT_THREAD_DEBUG_EVENT:
            threads.erase(event.dwThreadId);
            current = *threads.start();
            break;
         case LOAD_DLL_DEBUG_EVENT:
            ::CloseHandle(event.u.LoadDll.hFile);
            break;
         case UNLOAD_DLL_DEBUG_EVENT:
            break;
         case OUTPUT_DEBUG_STRING_EVENT:
            break;
         case RIP_EVENT:
            started = false;
            break;
         case EXCEPTION_DEBUG_EVENT:
            current = threads.get(dwCurrentThreadId);
            if (current) {
               current->refresh();
               processException(&event.u.Exception);
               current->refresh();
            }
            break;
      }
   }
}

void Debugger :: processException(EXCEPTION_DEBUG_INFO* exception)
{
   switch (exception->ExceptionRecord.ExceptionCode) {
      case EXCEPTION_SINGLE_STEP:
         if (breakpoints.processStep(current, stepMode))
            break;

         // stop if it is VM Hook mode
         if (init_breakpoint == -1) {
            init_breakpoint = current->context.Eip;
            trapped = true;
         }
         else if (current->context.Eip >= minAddress && current->context.Eip <= maxAddress) {
            processStep();
         }
         if (!trapped)
            current->setTrapFlag();

         break;
      case EXCEPTION_BREAKPOINT:
         if (breakpoints.processBreakpoint(current)) {
            current->state = steps.get(current->context.Eip);
            trapped = true;
            stepMode = false;
            current->setTrapFlag();
         }
         else if (init_breakpoint != 0 && init_breakpoint != 0xFFFFFFFF) {
            trapped = true;
            init_breakpoint = current->context.Eip;
         }
         break;
      default:
         if (exception->dwFirstChance != 0) {
            needToHandle = true;
         }
         else {
            this->exception.code = exception->ExceptionRecord.ExceptionCode;
            this->exception.address = (int)exception->ExceptionRecord.ExceptionAddress;
            TerminateProcess(current->hProcess, 1);
         }
         break;
   }
}

void Debugger :: processStep()
{
   current->state = steps.get(current->context.Eip);
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
         current->checkFailed = (current->context.Eax == 0);
         current->atCheckPoint = false;
      }
      else current->checkFailed = false;

      return current->checkFailed;
   }
   else return exitCheckPoint;
}


void Debugger :: setAutoStepMode()
{
   current->autoStep = true;
}

bool Debugger :: checkAutoStep()
{
   if (current->autoStep) {
      current->autoStep = false;

      return true;
   }
   else return false;
}

void Debugger :: processVirtualStep(void* state)
{
   current->state = state;
}

void Debugger :: continueProcess()
{
   int code = needToHandle ? DBG_EXCEPTION_NOT_HANDLED : DBG_CONTINUE;

   ContinueDebugEvent(dwCurrentProcessId, dwCurrentThreadId, code);

   needToHandle = false; 
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
   // !! temporal
   current->clearHardwareBreakpoint();

   current->setTrapFlag();
   stepMode = true;
}

void Debugger :: setBreakpoint(size_t address, bool withStackLevelControl)
{
   breakpoints.setHardwareBreakpoint(address, current, withStackLevelControl);
}

void Debugger :: setCheckMode()
{
   current->setCheckPoint();
}

bool Debugger :: start(const wchar_t* exePath, const wchar_t* cmdLine)
{
   if (startProcess(exePath, cmdLine)) {
      processEvent(INFINITE);

      return true;
   }
   else return false;
}

bool Debugger :: proceed(size_t timeout)
{
   processEvent(timeout);

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

   if (current)
      ::TerminateProcess(current->hProcess, 1);

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
   HANDLE hThread = CreateThread(NULL, 4096,
                     (LPTHREAD_START_ROUTINE)debugEventThread,
                     (LPVOID)controller,
                     0, &threadId);

   if (!hThread) {
      return false;
   }
   else ::CloseHandle(hThread);

   return true;
}

void Debugger :: reset()
{
   trapped = false;

   threads.clear();
   current = NULL;

   minAddress = 0xFFFFFFFF;
   maxAddress = 0;

   steps.clear();
   breakpoints.clear();

   init_breakpoint = 0;
   stepMode = false;
   needToHandle = false;
   exitCheckPoint = false;

   _vmHook = 0;
}

BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam)
{
   if (GetWindowThreadProcessId(hwnd, NULL)==(DWORD)lParam) {
      _ELENA_::setForegroundWindow(hwnd);

      return FALSE;
   }
   else return TRUE;
}

void Debugger :: activate()
{
   if (started) {
      EnumWindows(EnumThreadWndProc, dwCurrentThreadId);
   }
}

size_t Debugger :: findEntryPoint(const wchar_t* programPath)
{
  return _ELENA_::PEHelper::findEntryPoint(programPath);
}

bool Debugger :: findSignature(StreamReader& reader, char* signature)
{
   size_t rdata = 0;
   PEHelper::seekSection(reader, ".rdata", rdata);

   // load Executable image
   Context()->readDump(rdata + 4, signature, strlen(ELENACLIENT_SIGNITURE));
   signature[strlen(ELENACLIENT_SIGNITURE)] = 0;

   return true;
}

bool Debugger :: initDebugInfo(bool standAlone, StreamReader& reader, size_t& debugInfoPtr)
{
   size_t rdata = 0;
   PEHelper::seekSection(reader, ".rdata", rdata);

   if (standAlone) {
      // is not supported for a standalone - dn file should be used 
      return false;
   }
   else {
      // read SystemEnv
      SystemEnv env;
      Context()->readDump(Context()->readDWord(rdata), (char*)&env, sizeof(SystemEnv));

      // read Table
      GCTable table;
      Context()->readDump((size_t)env.Table, (char*)&table, sizeof(GCTable));

      if (table.dbg_ptr != 0) {
         debugInfoPtr = table.dbg_ptr;

         return true;
      }
      else return false;
   }
}