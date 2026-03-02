//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the Linux Debugger adapter class implementation
//                                             (C)2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <sys/wait.h>
//#include <stddef.h>
#include <errno.h>
#include <sys/ptrace.h>

#include "lnxdebugadapter.h"
#include "lnxcontroller.h"
//#include "eng/messages.h"
#include "linux/elfhelper.h"
#include "engine/core.h"

using namespace elena_lang;

#if defined _M_IX86 || __i386__

typedef VMTHeader32     VMTHeader;
typedef ObjectPage32    ObjectHeader;

constexpr auto elVMTFlagOffset   = elVMTFlagOffset32;
constexpr auto elObjectOffset    = elObjectOffset32;
constexpr auto elStructMask      = elStructMask32;

#elif defined _M_X64 || __x86_64__

typedef VMTHeader64     VMTHeader;
typedef ObjectPage64    ObjectHeader;

constexpr auto elVMTFlagOffset   = elVMTFlagOffset64;
constexpr auto elObjectOffset    = elObjectOffset64;
constexpr auto elStructMask      = elStructMask64;

#endif

// --- ThreadContext ---

ThreadContext :: ThreadContext(pid_t pid)
{
   this->_threadId = pid;
   this->_state = nullptr;
   this->_stepMode = false;
}

void ThreadContext :: refresh()
{
   ptrace(PTRACE_GETREGS, _threadId, NULL, &_context);
}

bool ThreadContext :: readDump(addr_t address, char* dump, size_t length)
{
   int index = 0;
   long val;
   while (length > 0) {
      val = ptrace(PTRACE_PEEKDATA,
                          _threadId, address + index * 4,
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

   return true;
}

void ThreadContext :: writeDump(addr_t address, char* dump, size_t length)
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
                          _threadId, address + index * 4,
                          NULL);

         memcpy(&val, dump + index * 4, length);
         length = 0;
      }

      ptrace(PTRACE_POKEDATA,
                          _threadId, address + index * 4,
                          (void*)val);

      index++;
   }
}

char ThreadContext :: setSoftwareBreakpoint(addr_t breakpoint)
{
   unsigned char code = 0;
   unsigned char terminator = 0xCC;

   readDump(breakpoint, (char*)&code, 1);
   writeDump(breakpoint, (char*)&terminator, 1);

   return code;
}

void ThreadContext :: clearSoftwareBreakpoint(addr_t breakpoint, char substitute)
{
   writeDump(breakpoint, &substitute, 1);
}

void ThreadContext :: setTrapFlag()
{
   _stepMode = true;
}

void ThreadContext :: resetTrapFlag()
{
   _stepMode = false;
}

#if defined _M_IX86 || __i386__

bool ThreadContext :: checkStepRange(addr_t minAddress, add_t maxAddress)
{
   return _context.eip >= minAddress && _context.eip <= maxAddress;
}

addr_t ThreadContext :: IP()
{
   return _context.eip;
}

void ThreadContext :: setIP(size_t address)
{
   ptrace(PTRACE_POKEUSER, _threadId,
                  offsetof(struct user, regs.eip), address);
}

addr_t ThreadContext :: BP()
{
   return _context.ebp;
}

#elif defined _M_X64 || __x86_64__

bool ThreadContext :: checkStepRange(addr_t minAddress, addr_t maxAddress)
{
   return _context.rip >= minAddress && _context.rip <= maxAddress;
}

addr_t ThreadContext :: IP()
{
   return _context.rip;
}

void ThreadContext :: setIP(addr_t address)
{
   ptrace(PTRACE_POKEUSER, _threadId,
                  offsetof(struct user, regs.rip), address);
}

addr_t ThreadContext :: BP()
{
   return _context.rbp;
}

#endif

// --- BreakpointController ---

BreakpointController :: BreakpointController()
   : breakpoints(0)
{
}

void BreakpointController :: setTempBreakpoint(addr_t address, ThreadContext* context)
{
   TempBreakpoint breakpoint = { address };
   breakpoint.substitute = context->setSoftwareBreakpoint(address);
   breakpoint.mode = TempBreakpoint::Mode::Software;

   for (size_t i = 0; i < tempBreakpoints.count(); i++) {
      if (!tempBreakpoints[i].isAssigned()) {
         tempBreakpoints[i] = breakpoint;

         return;
      }
   }

   tempBreakpoints.add(breakpoint);
}

bool BreakpointController :: clearTempBreakpoint(addr_t address, ThreadContext* context)
{
   bool proceeded = false;

   for (size_t i = 0; i < tempBreakpoints.count(); i++) {
      if (tempBreakpoints[i].isAssigned() && tempBreakpoints[i].address == address) {
         context->clearSoftwareBreakpoint(address, tempBreakpoints[i].substitute);

         tempBreakpoints[i].reset();

         proceeded = true;
      }
   }

   return proceeded;
}

void BreakpointController :: addBreakpoint(addr_t address, ThreadContext* context, bool started)
{
   if (started) {
      breakpoints.add(address, context->setSoftwareBreakpoint(address));
   }
   else breakpoints.add(address, 0);
}

void BreakpointController :: removeBreakpoint(addr_t address, ThreadContext* context, bool started)
{
   if (started) {
      context->clearSoftwareBreakpoint(address, breakpoints.get(address));
      if (context->_resetBreakpoint.mode == TempBreakpoint::Mode::Reset && context->_resetBreakpoint.address == address) {
         context->_resetBreakpoint.reset();
         context->resetTrapFlag();
      }
   }
   breakpoints.erase(address);
}

void BreakpointController :: setSoftwareBreakpoints(ThreadContext* context)
{
   for(auto breakpoint = breakpoints.start(); !breakpoint.eof(); ++breakpoint) {
      *breakpoint = context->setSoftwareBreakpoint(breakpoint.key());
   }
}

bool BreakpointController :: processBreakpoint(ThreadContext* context)
{
   bool proceeded = false;

   addr_t address = context->IP() - 1;

   if (clearTempBreakpoint(address, context)) {
      proceeded = true;
   }

   if (breakpoints.exist(address)) {
      TempBreakpoint resetBreakpoint(address, TempBreakpoint::Mode::Reset);
      context->_resetBreakpoint = resetBreakpoint;

      if (!proceeded) {
         char substitute = breakpoints.get(resetBreakpoint.address);
         context->clearSoftwareBreakpoint(resetBreakpoint.address, substitute);

         proceeded = true;
      }
   }

   if (proceeded) {
      context->setIP(address);

      return true;
   }

   return false;
}

bool BreakpointController :: processStep(ThreadContext* context)
{
   if (context->_resetBreakpoint.mode == TempBreakpoint::Mode::Reset) {
      // reset the breakpoint if required
      context->setSoftwareBreakpoint(context->_resetBreakpoint.address);

      //if (stepMode)
      //   context->setTrapFlag();

      context->_resetBreakpoint.mode = TempBreakpoint::Mode::None;
   }
   else return false;

   return true;
}

void BreakpointController :: clear()
{
   breakpoints.clear();
}

// --- ProcessException ---

//const char* ProcessException :: Text()
//{
//   switch (code) {
//      case SIGSEGV:
//         return ACCESS_VIOLATION_EXCEPTION_TEXT;
////      case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
////         return ARRAY_BOUNDS_EXCEEDED_EXCEPTION_TEXT;
////      case EXCEPTION_DATATYPE_MISALIGNMENT:
////         return DATATYPE_MISALIGNMENT_EXCEPTION_TEXT;
////      case EXCEPTION_FLT_DENORMAL_OPERAND:
////         return FLT_DENORMAL_OPERAND_EXCEPTION_TEXT;
////      case EXCEPTION_FLT_DIVIDE_BY_ZERO:
////         return FLT_DIVIDE_BY_ZERO_EXCEPTION_TEXT;
////      case EXCEPTION_FLT_INEXACT_RESULT:
////         return FLT_INEXACT_RESULT_EXCEPTION_TEXT;
////      case EXCEPTION_FLT_INVALID_OPERATION:
////         return FLT_INVALID_OPERATION_EXCEPTION_TEXT;
////      case EXCEPTION_FLT_OVERFLOW:
////         return FLT_OVERFLOW_EXCEPTION_TEXT;
////      case EXCEPTION_FLT_STACK_CHECK:
////         return FLT_STACK_CHECK_EXCEPTION_TEXT;i
////      case EXCEPTION_FLT_UNDERFLOW:
////         return FLT_UNDERFLOW_EXCEPTION_TEXT;
////      case EXCEPTION_ILLEGAL_INSTRUCTION:
////         return ILLEGAL_INSTRUCTION_EXCEPTION_TEXT;
////      case EXCEPTION_IN_PAGE_ERROR:
////         return PAGE_ERROR_EXCEPTION_TEXT;
////      case EXCEPTION_INT_DIVIDE_BY_ZERO:
////         return INT_DIVIDE_BY_ZERO_EXCEPTION_TEXT;
////      case EXCEPTION_INT_OVERFLOW:
////         return INT_OVERFLOW_EXCEPTION_TEXT;
////      case EXCEPTION_INVALID_DISPOSITION:
////         return INVALID_DISPOSITION_EXCEPTION_TEXT;
////      case EXCEPTION_NONCONTINUABLE_EXCEPTION:
////         return NONCONTINUABLE_EXCEPTION_EXCEPTION_TEXT;
////      case EXCEPTION_PRIV_INSTRUCTION:
////         return PRIV_INSTRUCTION_EXCEPTION_TEXT;
////      case EXCEPTION_STACK_OVERFLOW:
////         return STACK_OVERFLOW_EXCEPTION_TEXT;
////      case ELENA_ERR_OUTOF_MEMORY:
////         return GC_OUTOF_MEMORY_EXCEPTION_TEXT;
//      default:
//         return UNKNOWN_EXCEPTION_TEXT;
//   }
//}

// --- DebugProcessController ---

DebugProcessController :: DebugProcessController()
   : _threads(nullptr), _steps(nullptr), _exception({})
{
   _currentId = _traceeId = 0;
   _current = nullptr;

   _trapped = _started = false;
   _init_breakpoint = 0;
   _minAddress = INVALID_ADDR;
   _maxAddress = 0;
}

bool DebugProcessController :: startProcess(const char* exePath, const char* cmdLine, const char* appPath)
{
   _traceeId = fork();
   if (_traceeId >= 0) {  /* fork succeeded */
      if (_traceeId == 0) { /* fork() returns 0 for the child process */
         ptrace(PTRACE_TRACEME, 0, NULL, NULL);

         char* const* args = (char* const*)LinuxProcess::generateArguments(cmdLine);
         int retVal = execv(exePath, args);
      }
      else { /* parent process */
         _started = true;
         _exception.code = 0;

         // enabling multi-threading debugging
         ptrace(PTRACE_SETOPTIONS, _traceeId, NULL, PTRACE_O_TRACECLONE/* | PTRACE_O_TRACEFORK*/);

         _current = new ThreadContext(_traceeId);
         _threads.add(_traceeId, _current);

         _breakpoints.setSoftwareBreakpoints(_current);
      }
   }
   else return false;

   return true;
}

void DebugProcessController :: processEvent()
{
   _trapped = false;

   int status;
   _currentId = waitpid(-1, &status, __WALL);
   if (_currentId == -1)
      return;

   // new thread
   if(WIFSTOPPED(status) && WSTOPSIG(status) == SIGTRAP) {
      if(((status >> 16) & 0xffff) == PTRACE_EVENT_CLONE) {
         pid_t newThreadId;
         if(ptrace(PTRACE_GETEVENTMSG, _currentId, 0, &newThreadId) != -1) {
            _current = new ThreadContext(newThreadId);
            _current->refresh();

            _threads.add(newThreadId, _current);
         }
      }
   }

   // thread closed / killed
   if (WIFEXITED(status) || WIFSIGNALED(status)) {
      _current = _threads.get(_currentId);

      _threads.erase(_currentId);

      // process closed
      if (_threads.count() == 0) {
         if (_current) {
            _current->refresh();
         }

         _started = false;
      }

      _current = nullptr;
   }
   else if (WIFSTOPPED(status)) {
      _current = _threads.get(_currentId);
      if (_current)
         _current->refresh();

      int stopCode = WSTOPSIG(status);
      processSignal(stopCode);
   }
}

void DebugProcessController :: processSignal(int signal)
{
   if(signal == SIGTRAP && _current) {
      if (_breakpoints.processBreakpoint(_current)) {
         _current->_state = _steps.get(_current->IP());
         _trapped = true;
         _current->setTrapFlag();
      }
      else if (_breakpoints.processStep(_current)) {
         return;
      }
      else {
         if (_current->checkStepRange(_minAddress, _maxAddress)) {
            processStep();
         }
         if (!_trapped)
            _current->setTrapFlag();
      }
   }
   else if (signal == SIGSEGV) {
      struct __ptrace_peeksiginfo_args mask;
      siginfo_t info;

      mask.nr = 1;
      mask.flags = 0;
      mask.off = 0;

      ptrace(PTRACE_PEEKSIGINFO, _currentId, &mask, &info);

      this->_exception.code = signal;
      this->_exception.address = (addr_t)info.si_addr;
   }
}

void DebugProcessController :: processStep()
{
   _current->_state = _steps.get(_current->IP());
   if (_current->_state != nullptr) {
      _trapped = true;
      _current->resetTrapFlag();
      //proceedCheckPoint();
   }
}

bool DebugProcessController :: isInitBreakpoint()
{
   return _current ? _init_breakpoint == _current->IP() : false;
}

addr_t DebugProcessController :: getBaseAddress()
{
   return 0x08048000u; // !! temporal
}

bool DebugProcessController :: findSignature(StreamReader& reader, char* signature, pos_t length)
{
   if (!_current)
      return false;

   reader.seek(0x08048000u);

   size_t rva = 0;
   ELFHelper::seekRDataSegment(reader, rva);

   // load Executable image
   _current->readDump(rva, signature, length);
   signature[length] = 0;

   return true;
}

void* DebugProcessController :: getState()
{
   return _current ? _current->_state : nullptr;
}

addr_t DebugProcessController :: getStackFrame()
{
   return _current ? _current->BP() : 0;
}

addr_t DebugProcessController :: getIP()
{
   return _current ? _current->IP() : 0;
}

addr_t DebugProcessController :: getMemoryPtr(addr_t address)
{
   addr_t retPtr = 0;

   if (_current && _current->readDump(address, (char*)&retPtr, sizeof(addr_t))) {
      return retPtr;
   }
   else return 0;
}

ref_t DebugProcessController :: getMemoryRef(addr_t address)
{
   ref_t retPtr = 0;

   if (_current && _current->readDump(address, (char*)&retPtr, sizeof(ref_t))) {
      return retPtr;
   }
   else return 0;
}

void DebugProcessController :: addBreakpoint(addr_t address)
{
   _breakpoints.addBreakpoint(address, _current, _started);
}

void DebugProcessController :: removeBreakpoint(addr_t address)
{
   _breakpoints.removeBreakpoint(address, _current, _started);
}

void DebugProcessController :: continueProcess()
{
/*   if (_current) {
      if (breakpoints.applyPendingBreakpoints(current))
         stepMode = false;
   }*/

   ptrace((_current && _current->_stepMode) ? PTRACE_SINGLESTEP : PTRACE_CONT, _currentId, nullptr, nullptr);
}

void DebugProcessController :: stop()
{
   if (!_started)
      return;

   kill(_traceeId, SIGKILL);

   continueProcess();
}

void DebugProcessController :: reset()
{
   _trapped = false;

   _threads.clear();
   _current = nullptr;

   _init_breakpoint = 0;
   _minAddress = INVALID_ADDR;
   _maxAddress = 0;

   _steps.clear();
   _breakpoints.clear();
}

void DebugProcessController :: setStepMode()
{
   if (_current)
      _current->setTrapFlag();
}

void DebugProcessController :: addStep(addr_t address, void* state)
{
   _steps.add(address, state);
   if (address < _minAddress)
      _minAddress = address;

   if (address > _maxAddress)
      _maxAddress = address;
}

void DebugProcessController :: setBreakpoint(addr_t address)
{
   if (_current) {
      _current->resetTrapFlag();

      _breakpoints.setTempBreakpoint(address, _current);
   }
}

void DebugProcessController :: resetException()
{
   _exception.code = 0;
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
   : /*_debugProcess(CONSOLE_OUTPUT_TEXT), */_threadId(0)
{
}

bool LnxDebugAdapter :: isStarted()
{
   return _process.isStarted();
}

bool LnxDebugAdapter :: isTrapped()
{
   return _process.isTrapped();
}

bool LnxDebugAdapter :: isInitBreakpoint()
{
   return _process.isInitBreakpoint();
}

int LnxDebugAdapter :: getDataOffset()
{
   return sizeof(addr_t);
}

addr_t LnxDebugAdapter :: getBaseAddress()
{
   return _process.getBaseAddress();
}

void* LnxDebugAdapter :: getState()
{
   return _process.getState();
}

void* LnxDebugAdapter :: retrieveState(addr_t address)
{
   return _process.retrieveState(address);
}

addr_t LnxDebugAdapter :: getFrame()
{
   return _process.getStackFrame();
}

addr_t LnxDebugAdapter :: getIP()
{
   return _process.getIP();
}

addr_t LnxDebugAdapter :: getStackItem(int index, disp_t offset)
{
   return _process.getMemoryPtr(getStackItemAddress(index * sizeof(addr_t) + offset));
}

addr_t LnxDebugAdapter :: getStackItemAddress(disp_t disp)
{
   return getFrame() - disp;
}

addr_t LnxDebugAdapter :: getField(addr_t address, int index)
{
   disp_t offset = index * sizeof(addr_t);

   return _process.getMemoryPtr(address + offset);
}

addr_t LnxDebugAdapter :: getFieldAddress(addr_t address, disp_t disp)
{
   return address + disp;
}

addr_t LnxDebugAdapter :: getClassVMT(addr_t address)
{
   return _process.getMemoryPtr(address - elObjectOffset);
}

ref_t LnxDebugAdapter :: getClassFlags(addr_t vmtAddress)
{
   return _process.getMemoryRef(vmtAddress - elVMTFlagOffset);
}

size_t LnxDebugAdapter :: getArrayLength(addr_t address)
{
   ObjectHeader header;
   if (_process.read(address - elObjectOffset, header)) {
      return header.size & ~elStructMask;
   }

   return 0;
}

char LnxDebugAdapter :: getBYTE(addr_t address)
{
   char value;
   if (_process.read(address, value)) {
      return value;
   }

   return 0;
}

unsigned short LnxDebugAdapter :: getWORD(addr_t address)
{
   unsigned short value;
   if (_process.read(address, value)) {
      return value;
   }

   return 0;
}

unsigned int LnxDebugAdapter :: getDWORD(addr_t address)
{
   unsigned int value;
   if (_process.read(address, value)) {
      return value;
   }

   return 0;
}

unsigned long long LnxDebugAdapter :: getQWORD(addr_t address)
{
   unsigned long long value;
   if (_process.read(address, value)) {
      return value;
   }

   return 0;
}

double LnxDebugAdapter :: getFLOAT64(addr_t address)
{
   double value;
   if (_process.read(address, value)) {
      return value;
   }

   return 0;}

void LnxDebugAdapter :: addBreakpoint(addr_t address)
{
   _process.addBreakpoint(address);
}

void LnxDebugAdapter :: removeBreakpoint(addr_t address)
{
   _process.removeBreakpoint(address);
}

addr_t LnxDebugAdapter :: findEntryPoint(path_t programPath)
{
   return ELFHelper::findEntryPoint(programPath);
}

bool LnxDebugAdapter :: findSignature(StreamReader& reader, char* signature, pos_t length)
{
   return _process.findSignature(reader, signature, length);
}

void LnxDebugAdapter :: activate()
{
   //_process.activateWindow();
}

void LnxDebugAdapter :: run()
{
   _process.continueProcess();
}

bool LnxDebugAdapter :: proceed(int)
{
   _process.processEvent();

   return !_process.isTrapped();
}

void LnxDebugAdapter :: stop()
{
   _process.stop();
}

void LnxDebugAdapter :: reset()
{
   _process.reset();
}

void LnxDebugAdapter :: setStepMode()
{
   _process.setStepMode();
}

bool LnxDebugAdapter :: readDump(addr_t address, char* s, pos_t length)
{
   return _process.read(address, s, length);
}

void LnxDebugAdapter :: addStep(addr_t address, void* current)
{
   _process.addStep(address, current);
}

void LnxDebugAdapter :: setBreakpoint(addr_t address, bool)
{
   _process.setBreakpoint(address);
}

bool LnxDebugAdapter :: startThread(DebugControllerBase* controller)
{
   int err = pthread_create(&_threadId, nullptr, &debugEventThread, controller);

   if (err != 0) {
      return false;
   }

   return true;
}

DebugProcessException* LnxDebugAdapter :: Exception()
{
   auto debugException = _process.getException();
   if (debugException) {
      _exception.address = debugException->address;
      _exception.code = debugException->code;

      return &_exception;
   }

   return nullptr;
}

void LnxDebugAdapter :: resetException()
{
   _exception.address = 0;
   _exception.code = 0;

   _process.resetException();
}

bool LnxDebugAdapter :: startProgram(path_t exePath, path_t cmdLine, path_t appPath, StartUpSettings&)
{
   if (_process.startProcess(exePath.str(), cmdLine.str(), appPath.str()))
   {
      _process.processEvent();

      return true;
   }

   return false;
}
