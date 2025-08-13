//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Win32 Debugger class implementation
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "common.h"
// -------------------------------------------------------------------------
#include "win32debugprocess.h"

#include "lruntime\windows\pehelper.h"
#include "engine\core.h"

using namespace elena_lang;

#ifdef _M_IX86

typedef unsigned long   SIZE_T;
typedef VMTHeader32     VMTHeader;
typedef ObjectPage32    ObjectHeader;

constexpr auto elVMTFlagOffset   = elVMTFlagOffset32;
constexpr auto elObjectOffset    = elObjectOffset32;
constexpr auto elStructMask      = elStructMask32;

static inline addr_t getIP(CONTEXT& context)
{
   return context.Eip;
}

static inline addr_t getBP(CONTEXT& context)
{
   return context.Ebp;
}

static inline void setIP(CONTEXT& context, addr_t address)
{
   context.Eip = address;
}

#elif _M_X64

typedef size_t          SIZE_T;
typedef VMTHeader64     VMTHeader;
typedef ObjectPage64    ObjectHeader;

constexpr auto elVMTFlagOffset   = elVMTFlagOffset64;
constexpr auto elObjectOffset    = elObjectOffset64;
constexpr auto elStructMask      = elStructMask64;

static inline void setIP(CONTEXT& context, addr_t address)
{
   context.Rip = address;
}

static inline addr_t getIP(CONTEXT& context)
{
   return context.Rip;
}

static inline addr_t getBP(CONTEXT& context)
{
   return context.Rbp;
}

#endif

// --- setForegroundWindow() ---
inline void setForegroundWindow(HWND hwnd)
{
   size_t dwTimeoutMS = 0;
   // Get the current lock timeout.
   ::SystemParametersInfo(0x2000, 0, &dwTimeoutMS, 0);

   // Set the lock timeout to zero
   ::SystemParametersInfo(0x2001, 0, 0, 0);

   // Perform the SetForegroundWindow
   ::SetForegroundWindow(hwnd);

   // Set the timeout back
   ::SystemParametersInfo(0x2001, 0, (LPVOID)dwTimeoutMS, 0);   //HWND hCurrWnd;
}

BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam)
{
   if (GetWindowThreadProcessId(hwnd, NULL) == (DWORD)lParam) {
      setForegroundWindow(hwnd);

      return FALSE;
   }
   else return TRUE;
}

// --- Win32BreakpointContext ---

Win32BreakpointContext :: Win32BreakpointContext()
   : breakpoints(0)
{   
}

bool Win32BreakpointContext :: processStep(Win32ThreadContext* context, bool stepMode)
{
   if (context->resetBreakpoint.mode == Win32TempBreakpoint::Mode::Reset) {
      // reset the breakpoint if required
      context->setSoftwareBreakpoint(context->resetBreakpoint.address);

      if (stepMode)
         context->setTrapFlag();

      context->resetBreakpoint.mode = Win32TempBreakpoint::Mode::None;
   }
   else return false;

   return true;
}

void Win32BreakpointContext :: setTempBreakpoint(addr_t address, Win32ThreadContext* context)
{
   Win32TempBreakpoint breakpoint = { address };
   breakpoint.substitute = context->setSoftwareBreakpoint(address);
   breakpoint.mode = Win32TempBreakpoint::Mode::Software;

   for (size_t i = 0; i < tempBreakpoints.count(); i++) {
      if (!tempBreakpoints[i].isAssigned()) {
         tempBreakpoints[i] = breakpoint;

         return;
      }
   }

   tempBreakpoints.add(breakpoint);
}

bool Win32BreakpointContext :: clearTempBreakpoint(addr_t address, Win32ThreadContext* context)
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

void Win32BreakpointContext :: addBreakpoint(addr_t address, Win32ThreadContext* context, bool started)
{
   if (started) {
      breakpoints.add(address, context->setSoftwareBreakpoint(address));
   }
   else breakpoints.add(address, 0);
}

void Win32BreakpointContext :: removeBreakpoint(addr_t address, Win32ThreadContext* context, bool started)
{
   if (started) {
      context->clearSoftwareBreakpoint(address, breakpoints.get(address));
      if (context->resetBreakpoint.mode == Win32TempBreakpoint::Mode::Reset && context->resetBreakpoint.address == address) {
         context->resetBreakpoint.reset();
         context->resetTrapFlag();
      }
   }
   breakpoints.erase(address);
}

void Win32BreakpointContext :: setSoftwareBreakpoints(Win32ThreadContext* context)
{
   Map<size_t, char>::Iterator breakpoint = breakpoints.start();
   while (!breakpoint.eof()) {
      *breakpoint = context->setSoftwareBreakpoint(breakpoint.key());

      breakpoint++;
   }
}

bool Win32BreakpointContext :: processBreakpoint(Win32ThreadContext* context)
{
   bool proceeded = false;

   addr_t address = getIP(context->context) - 1;

   if (clearTempBreakpoint(address, context)) {
      proceeded = true;
   }

   if (breakpoints.exist(address)) {
      Win32TempBreakpoint resetBreakpoint(address, Win32TempBreakpoint::Mode::Reset);
      context->resetBreakpoint = resetBreakpoint;

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

void Win32BreakpointContext :: clear()
{
   breakpoints.clear();
}

// --- Win32ThreadContext ---

Win32ThreadContext :: Win32ThreadContext(HANDLE hProcess, HANDLE hThread)
   : context({}), resetBreakpoint()
{
   this->hProcess = hProcess;
   this->hThread = hThread;
   this->state = nullptr;
}

void Win32ThreadContext :: refresh()
{
   context.ContextFlags = CONTEXT_FULL;
   GetThreadContext(hThread, &context);
   if (context.SegFs == 0) {                                 // !! hotfix
      context.SegFs = 0x38;
      SetThreadContext(hThread, &context);
   }
}

bool Win32ThreadContext :: readDump(addr_t address, char* dump, size_t length)
{
   SIZE_T size = 0;

   ReadProcessMemory(hProcess, (void*)(address), dump, length, &size);

   return size != 0;
}

void Win32ThreadContext :: writeDump(addr_t address, char* dump, size_t length)
{
   SIZE_T size = 0;

   WriteProcessMemory(hProcess, (void*)(address), dump, length, &size);
}

unsigned char Win32ThreadContext :: setSoftwareBreakpoint(addr_t breakpoint)
{
   unsigned char code = 0;
   unsigned char terminator = 0xCC;

   readDump(breakpoint, (char*)&code, 1);
   writeDump(breakpoint, (char*)&terminator, 1);

   return code;
}

void Win32ThreadContext :: clearSoftwareBreakpoint(addr_t breakpoint, unsigned char substitute)
{
   writeDump(breakpoint, (char*)&substitute, 1);
}

void Win32ThreadContext :: setHardwareBreakpoint(addr_t breakpoint)
{
   context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
   context.Dr0 = breakpoint;
   context.Dr7 = 0x000001;
   SetThreadContext(hThread, &context);
}

void Win32ThreadContext :: clearHardwareBreakpoint()
{
   context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
   context.Dr0 = 0x0;
   context.Dr7 = 0x0;
   SetThreadContext(hThread, &context);
}

void Win32ThreadContext::resetTrapFlag()
{
   context.ContextFlags = CONTEXT_CONTROL;
   context.EFlags &= ~0x100;
   SetThreadContext(hThread, &context);
}

void Win32ThreadContext::setTrapFlag()
{
   context.ContextFlags = CONTEXT_CONTROL;
   context.EFlags |= 0x100;
   SetThreadContext(hThread, &context);
}

void Win32ThreadContext :: setIP(addr_t address)
{
   context.ContextFlags = CONTEXT_CONTROL;
   GetThreadContext(hThread, &context);
   ::setIP(context, address);
   SetThreadContext(hThread, &context);
}

// --- Win32DebugProcess::ConsoleHelper ---

void Win32DebugProcess::ConsoleHelper :: printText(const char* s)
{
   HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

   WriteConsoleA(output_handle, s, getlength_pos(s), 0, 0);
}

void Win32DebugProcess::ConsoleHelper :: waitForAnyKey()
{
   HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE);
   INPUT_RECORD input_record;
   DWORD input_length = 1;
   DWORD events_read = 0;

   while (true) {
      ReadConsoleInput(input_handle, &input_record, input_length, &events_read);

      if (input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown) {
         return;
      }
   }
}

// --- Win32DebugProcess ---

Win32DebugProcess :: Win32DebugProcess(const char* endingMessage)
   : _threads(nullptr), _steps(nullptr), _exception({})
{
   _needToFreeConsole = false;
   _started = false;

   _endingMessage = endingMessage;

   reset();
}

void Win32DebugProcess :: reset()
{
   _trapped = false;
   _newThread = false;
   
   _threads.clear();
   _current = nullptr;
   
   _minAddress = INVALID_ADDR;
   _maxAddress = 0;
   _baseAddress = 0;
   
   _dwDebugeeProcessId = 0;
   
   _steps.clear();
   _breakpoints.clear();
   
   _init_breakpoint = 0;
   _stepMode = false;
   _needToHandle = false;
   
   //_vmHook = 0;
}

inline bool isIncluded(path_t paths, path_t path)
{
   pos_t length = path.length_pos();
   pos_t index = paths.findStr(path);

   return (index != NOTFOUND_POS && (paths[index + length] == ';' || paths[index + length] == 0 || paths[index + length] == '\\'));
}

bool Win32DebugProcess :: startProcess(const wchar_t* exePath, const wchar_t* cmdLine, const wchar_t* appPath,
   bool includeAppPath2Paths, bool withExplicitConsole)
{
   DynamicString<path_c> pathsEnv;

   PROCESS_INFORMATION pi = { nullptr, nullptr, 0, 0 };
   STARTUPINFO         si;
   PathString          currentPath;
   DWORD               flags = DEBUG_PROCESS;

   currentPath.copySubPath(exePath, false);

   memset(&si, 0, sizeof(si));

   si.dwFlags = STARTF_USESHOWWINDOW;
   si.wShowWindow = SW_SHOWNORMAL;

   if (withExplicitConsole) {
      AllocConsole();

      _needToFreeConsole = true;
   }
   else flags |= CREATE_NEW_CONSOLE;

   size_t trimPos = NOTFOUND_POS;
   if (includeAppPath2Paths) {
      flags |= CREATE_UNICODE_ENVIRONMENT;

      pathsEnv.allocate(4096);

      int dwRet = GetEnvironmentVariable(L"PATH", (LPWSTR)pathsEnv.str(), 4096);
      if (dwRet && !isIncluded(pathsEnv.str(), appPath)) {
         trimPos = pathsEnv.length();

         if (!pathsEnv.empty() && pathsEnv[pathsEnv.length() - 1] != ';')
            pathsEnv.append(';');
         pathsEnv.append(appPath);

         SetEnvironmentVariable(L"PATH", pathsEnv.str());
      }
   }

   bool retVal = CreateProcess(
      exePath,
      (wchar_t*)cmdLine,
      nullptr,
      nullptr,
      FALSE,
      flags,
      nullptr,
      currentPath.str(), &si, &pi);

   if (trimPos != NOTFOUND_POS) {
      // rolling back changes to ENVIRONENT if required
      pathsEnv.trim(trimPos);

      SetEnvironmentVariable(L"PATH", pathsEnv.str());
   }

   if (!retVal) {
      return false;
   }

   _dwDebugeeProcessId = pi.dwProcessId;

   if (pi.hProcess)
      CloseHandle(pi.hProcess);

   if (pi.hThread)
      CloseHandle(pi.hThread);

   _started = true;
   _exception.code = 0;
   _needToHandle = false;

   return true;
}

void Win32DebugProcess :: processEvent(DWORD timeout)
{
   DEBUG_EVENT event;

   _trapped = false;
   if (WaitForDebugEvent(&event, timeout)) {
      _dwCurrentThreadId = event.dwThreadId;
      _dwCurrentProcessId = event.dwProcessId;

      switch (event.dwDebugEventCode) {
         case CREATE_PROCESS_DEBUG_EVENT:
            _current = new Win32ThreadContext(event.u.CreateProcessInfo.hProcess, event.u.CreateProcessInfo.hThread);
            _current->refresh();

            _threads.add(_dwCurrentThreadId, _current);

            if (_dwCurrentProcessId == _dwDebugeeProcessId) {
               _baseAddress = (addr_t)event.u.CreateProcessInfo.lpBaseOfImage;
               _breakpoints.setSoftwareBreakpoints(_current);
            }

            _newThread = true;

            ::CloseHandle(event.u.CreateProcessInfo.hFile);
            break;
         case EXIT_PROCESS_DEBUG_EVENT:
            _current = _threads.get(_dwCurrentThreadId);
            if (_current) {
               _current->refresh();
               //exitCheckPoint = proceedCheckPoint();
            }
            processEnd();
            break;
         case CREATE_THREAD_DEBUG_EVENT:
            _current = new Win32ThreadContext((*_threads.start())->hProcess, event.u.CreateThread.hThread);
            _current->refresh();

            _threads.add(_dwCurrentThreadId, _current);
            break;
         case EXIT_THREAD_DEBUG_EVENT:
            _threads.erase(event.dwThreadId);
            _current = *_threads.start();
            break;
         case LOAD_DLL_DEBUG_EVENT:
            ::CloseHandle(event.u.LoadDll.hFile);
            break;
         case UNLOAD_DLL_DEBUG_EVENT:
            break;
         case OUTPUT_DEBUG_STRING_EVENT:
            break;
         case RIP_EVENT:
            _started = false;
            break;
         case EXCEPTION_DEBUG_EVENT:
            _current = _threads.get(_dwCurrentThreadId);
            if (_current) {
               _current->refresh();
               processException(&event.u.Exception);
               _current->refresh();
            }
            break;
      }
   }
}

void Win32DebugProcess :: processEnd()
{
   _threads.clear();
   _current = nullptr;
   _started = false;
   if (_needToFreeConsole) {
      ConsoleHelper console;
      console.printText(_endingMessage);
      console.waitForAnyKey();

      FreeConsole();

      _needToFreeConsole = false;
   }
}

void Win32DebugProcess :: processException(EXCEPTION_DEBUG_INFO* exception)
{
   switch (exception->ExceptionRecord.ExceptionCode) {
      case EXCEPTION_SINGLE_STEP:
         if (_breakpoints.processStep(_current, _stepMode))
            break;

         // stop if it is VM Hook mode
         if (_init_breakpoint == INVALID_ADDR) {
            _init_breakpoint = getIP(_current->context);
            _trapped = true;
         }
         else if (getIP(_current->context) >= _minAddress && getIP(_current->context) <= _maxAddress) {
            processStep();
         }
         if (!_trapped)
            _current->setTrapFlag();

         break;
      case EXCEPTION_BREAKPOINT:
         if (_breakpoints.processBreakpoint(_current)) {
            _current->setTrapFlag();

            if (getIP(_current->context) >= _minAddress && getIP(_current->context) <= _maxAddress) {
               processStep();
            }
         }
         else if (_init_breakpoint != 0 && _init_breakpoint != INVALID_ADDR) {
            _trapped = true;
            _init_breakpoint = getIP(_current->context);
         }
         break;
      default:
         if (exception->dwFirstChance != 0) {
            _needToHandle = true;
         }
         else {
            this->_exception.code = exception->ExceptionRecord.ExceptionCode;
            this->_exception.address = (addr_t)exception->ExceptionRecord.ExceptionAddress;
            TerminateProcess(_current->hProcess, 1);
         }
         break;
   }
}

void Win32DebugProcess :: processStep()
{
   _current->state = _steps.get(getIP(_current->context));
   if (_current->state != nullptr) {
      _trapped = true;
      _stepMode = false;
      //proceedCheckPoint();
   }
}

void Win32DebugProcess :: continueProcess()
{
   int code = _needToHandle ? DBG_EXCEPTION_NOT_HANDLED : DBG_CONTINUE;

   ContinueDebugEvent(_dwCurrentProcessId, _dwCurrentThreadId, code);

   _needToHandle = false;
}

void Win32DebugProcess :: resetException()
{
   _exception.code = 0;
}

void Win32DebugProcess :: activateWindow()
{
   if (_started) {
      EnumWindows(EnumThreadWndProc, _dwCurrentThreadId);
   }
}

void Win32DebugProcess :: stop()
{
   if (!_started)
      return;
   
   if (_current)
      ::TerminateProcess(_current->hProcess, 1);
   
   continueProcess();
}

void Win32DebugProcess :: setBreakpoint(addr_t address, bool withStackLevelControl)
{
   _breakpoints.setTempBreakpoint(address, _current);
}

void Win32DebugProcess :: addBreakpoint(addr_t address)
{
   _breakpoints.addBreakpoint(address, _current, _started);
}

void Win32DebugProcess :: removeBreakpoint(addr_t address)
{
   _breakpoints.removeBreakpoint(address, _current, _started);
}

void Win32DebugProcess :: setStepMode()
{
   // !! temporal
   _current->clearHardwareBreakpoint();
   
   _current->setTrapFlag();
   _stepMode = true;
}

void Win32DebugProcess :: addStep(addr_t address, void* state)
{
   _steps.add(address, state);
   if (address < _minAddress)
      _minAddress = address;
   
   if (address > _maxAddress)
      _maxAddress = address;
}

addr_t Win32DebugProcess :: findEntryPoint(path_t programPath)
{
   return PEHelper::findEntryPoint(programPath);
}

bool Win32DebugProcess :: findSignature(StreamReader& reader, char* signature, pos_t length)
{
   size_t rdata = 0;
   if (!PEHelper::seekSection(reader, ".rdata", rdata))
      return false;
   
   // load Executable image
   _current->readDump(rdata + sizeof(addr_t), signature, length);
   
   return true;
}

addr_t Win32DebugProcess :: getClassVMT(addr_t address)
{
   addr_t ptr = 0;
   
   if (_current->readDump(address - elObjectOffset, (char*)&ptr, sizeof(addr_t))) {
      return ptr;
   }
   else return 0;
}

addr_t Win32DebugProcess :: getStackItemAddress(disp_t disp)
{
   return getBP(_current->context) - disp;
}

addr_t Win32DebugProcess :: getStackItem(int index, disp_t offset)
{
   return getMemoryPtr(getStackItemAddress(index * sizeof(addr_t) + offset));
}

addr_t Win32DebugProcess :: getMemoryPtr(addr_t address)
{
   addr_t retPtr = 0;
   
   if (_current->readDump(address, (char*)&retPtr, sizeof(addr_t))) {
      return retPtr;
   }
   else return 0;
}

unsigned short Win32DebugProcess :: getWORD(addr_t address)
{
   unsigned short word = 0;
   
   if (_current->readDump(address, (char*)&word, 2)) {
      return word;
   }
   else return 0;
}

unsigned Win32DebugProcess :: getDWORD(addr_t address)
{
   unsigned int dword = 0;
   
   if (_current->readDump(address, (char*)&dword, 4)) {
      return dword;
   }
   else return 0;
}

unsigned char Win32DebugProcess :: getBYTE(addr_t address)
{
   unsigned char b = 0;
   
   if (_current->readDump(address, (char*)&b, 1)) {
      return b;
   }
   else return 0;
}

unsigned long long Win32DebugProcess :: getQWORD(addr_t address)
{
   unsigned long long qword = 0;
   
   if (_current->readDump(address, (char*)&qword, 8)) {
      return qword;
   }
   else return 0;
}

double Win32DebugProcess :: getFLOAT64(addr_t address)
{
   double number = 0;
   
   if (_current->readDump(address, (char*)&number, 8)) {
      return number;
   }
   else return 0;
}

ref_t Win32DebugProcess :: getClassFlags(addr_t vmtAddress)
{
   ref_t flags = 0;
   if (_current->readDump(vmtAddress - elVMTFlagOffset, (char*)&flags, sizeof(flags))) {
      return flags;
   }
   else return 0;
}

addr_t Win32DebugProcess :: getField(addr_t address, int index)
{
   disp_t offset = index * sizeof(addr_t);
   
   return getMemoryPtr(address + offset);
}

addr_t Win32DebugProcess :: getFieldAddress(addr_t address, disp_t disp)
{
   return address + disp;
}

size_t Win32DebugProcess :: getArrayLength(addr_t address)
{
   ObjectHeader header;
   if (readDump(address - elObjectOffset, (char*)&header, elObjectOffset)) {
      return header.size & ~elStructMask;
   }

   return 0;
}

bool Win32DebugProcess :: isInitBreakpoint()
{
   return _current ? _init_breakpoint == getIP(_current->context) : false;
}
