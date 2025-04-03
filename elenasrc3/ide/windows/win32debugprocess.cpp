////---------------------------------------------------------------------------
////		E L E N A   P r o j e c t:  ELENA Engine
////               
////		This file contains the Win32 Debugger adapter class implementation
////                                             (C)2021-2025, by Aleksey Rakov
////---------------------------------------------------------------------------
//
//#include "common.h"
////---------------------------------------------------------------------------
//#include "windows/win32debugprocess.h"
//
//#include "core.h"
//#include "windows/pehelper.h"
//
////#ifdef _MSC_VER
////
////#include <tchar.h>
////
////#endif
////
////#include "eng/messages.h"
//
//using namespace elena_lang;
//
//#ifdef _M_IX86
//
//typedef unsigned long   SIZE_T;
//typedef VMTHeader32     VMTHeader;
//typedef ObjectPage32    ObjectHeader;
//
//constexpr auto elVMTFlagOffset   = elVMTFlagOffset32;
//constexpr auto elObjectOffset    = elObjectOffset32;
//constexpr auto elStructMask      = elStructMask32;
//
//inline addr_t getIP(CONTEXT& context)
//{
//   return context.Eip;
//}
//
//inline addr_t getBP(CONTEXT& context)
//{
//   return context.Ebp;
//}
//
//inline void setIP(CONTEXT& context, addr_t address)
//{
//   context.Eip = address;
//}
//
//#elif _M_X64
//
//typedef size_t          SIZE_T;
//typedef VMTHeader64     VMTHeader;
//typedef ObjectPage64    ObjectHeader;
//
//constexpr auto elVMTFlagOffset   = elVMTFlagOffset64;
//constexpr auto elObjectOffset    = elObjectOffset64;
//constexpr auto elStructMask      = elStructMask64;
//
//inline void setIP(CONTEXT& context, addr_t address)
//{
//   context.Rip = address;
//}
//
//inline addr_t getIP(CONTEXT& context)
//{
//   return context.Rip;
//}
//
//inline addr_t getBP(CONTEXT& context)
//{
//   return context.Rbp;
//}
//
//#endif
//
//// --- setForegroundWindow() ---
//inline void setForegroundWindow(HWND hwnd)
//{
//   size_t dwTimeoutMS = 0;
//   // Get the current lock timeout.
//   ::SystemParametersInfo(0x2000, 0, &dwTimeoutMS, 0);
//
//   // Set the lock timeout to zero
//   ::SystemParametersInfo(0x2001, 0, 0, 0);
//
//   // Perform the SetForegroundWindow
//   ::SetForegroundWindow(hwnd);
//
//   // Set the timeout back
//   ::SystemParametersInfo(0x2001, 0, (LPVOID)dwTimeoutMS, 0);   //HWND hCurrWnd;
//}
//
//BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam)
//{
//   if (GetWindowThreadProcessId(hwnd, NULL) == (DWORD)lParam) {
//      setForegroundWindow(hwnd);
//
//      return FALSE;
//   }
//   else return TRUE;
//}
//
//// --- main thread that is the debugger residing over a debuggee ---
//
//BOOL WINAPI debugEventThread(DebugControllerBase* controller)
//{
//   controller->debugThread();
//
//   ExitThread(TRUE);
//
//   return TRUE;
//}
//
//// --- Win32ThreadContext ---
//
//Win32ThreadContext :: Win32ThreadContext(HANDLE hProcess, HANDLE hThread)
//{
//   this->hProcess = hProcess;
//   this->hThread = hThread;
//   this->state = nullptr;
//}
//
//bool Win32ThreadContext :: readDump(addr_t address, char* dump, size_t length)
//{
//   SIZE_T size = 0;
//
//   ReadProcessMemory(hProcess, (void*)(address), dump, length, &size);
//
//   return size != 0;
//}
//
//void Win32ThreadContext :: writeDump(addr_t address, char* dump, size_t length)
//{
//   SIZE_T size = 0;
//
//   WriteProcessMemory(hProcess, (void*)(address), dump, length, &size);
//}
//
//void Win32ThreadContext :: refresh()
//{
//   context.ContextFlags = CONTEXT_FULL;
//   GetThreadContext(hThread, &context);
//   if (context.SegFs == 0) {                                 // !! hotfix
//      context.SegFs = 0x38;
//      SetThreadContext(hThread, &context);
//   }
//}
//
//unsigned char Win32ThreadContext :: setSoftwareBreakpoint(addr_t breakpoint)
//{
//   unsigned char code = 0;
//   unsigned char terminator = 0xCC;
//
//   readDump(breakpoint, (char*)&code, 1);
//   writeDump(breakpoint, (char*)&terminator, 1);
//
//   return code;
//}
//
//void Win32ThreadContext :: clearSoftwareBreakpoint(addr_t breakpoint, unsigned char substitute)
//{
//   writeDump(breakpoint, (char*)&substitute, 1);
//}
//
//void Win32ThreadContext :: setHardwareBreakpoint(addr_t breakpoint)
//{
//   context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
//   context.Dr0 = breakpoint;
//   context.Dr7 = 0x000001;
//   SetThreadContext(hThread, &context);
//   this->breakpoint.hardware = true;
//}
//
//void Win32ThreadContext :: clearHardwareBreakpoint()
//{
//   context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
//   context.Dr0 = 0x0;
//   context.Dr7 = 0x0;
//   SetThreadContext(hThread, &context);
//   breakpoint.hardware = false;
//}
//
//void Win32ThreadContext::resetTrapFlag()
//{
//   context.ContextFlags = CONTEXT_CONTROL;
//   context.EFlags &= ~0x100;
//   SetThreadContext(hThread, &context);
//}
//
//void Win32ThreadContext::setTrapFlag()
//{
//   context.ContextFlags = CONTEXT_CONTROL;
//   context.EFlags |= 0x100;
//   SetThreadContext(hThread, &context);
//}
//
//void Win32ThreadContext :: setIP(addr_t address)
//{
//   context.ContextFlags = CONTEXT_CONTROL;
//   GetThreadContext(hThread, &context);
//   ::setIP(context, address);
//   SetThreadContext(hThread, &context);
//}
//
//// --- Win32BreakpointContext ---
//
//Win32BreakpointContext :: Win32BreakpointContext()
//   : breakpoints(0)
//{
//   
//}
//
//void Win32BreakpointContext :: addBreakpoint(addr_t address, Win32ThreadContext* context, bool started)
//{
//   if (started) {
//      breakpoints.add(address, context->setSoftwareBreakpoint(address));
//   }
//   else breakpoints.add(address, 0);
//}
//
//void Win32BreakpointContext :: removeBreakpoint(addr_t address, Win32ThreadContext* context, bool started)
//{
//   if (started) {
//      context->clearSoftwareBreakpoint(address, breakpoints.get(address));
//      if (context->breakpoint.software && context->breakpoint.next == address) {
//
//         context->breakpoint.clearSoftware();
//         context->resetTrapFlag();
//      }
//   }
//   breakpoints.erase(address);
//}
//
//void Win32BreakpointContext :: setSoftwareBreakpoints(Win32ThreadContext* context)
//{
//   Map<size_t, char>::Iterator breakpoint = breakpoints.start();
//   while (!breakpoint.eof()) {
//      *breakpoint = context->setSoftwareBreakpoint(breakpoint.key());
//
//      breakpoint++;
//   }
//}
//
//bool Win32BreakpointContext :: processStep(Win32ThreadContext* context, bool stepMode)
//{
//   Win32ThreadBreakpoint breakpoint = context->breakpoint;
//
//   if (breakpoint.next != 0) {
//      if (breakpoint.software) {
//         context->setSoftwareBreakpoint(breakpoint.next);
//         context->breakpoint.software = false;
//      }
//      else context->setHardwareBreakpoint(breakpoint.next);
//      context->breakpoint.next = 0;
//      if (stepMode)
//         context->setTrapFlag();
//
//      return true;
//   }
//
//   if (breakpoint.hardware) {
//      context->clearHardwareBreakpoint();
//
//      // check stack level to skip recursive entries
//      if (getBP(context->context) < breakpoint.stackLevel) {
//         context->breakpoint.next = getIP(context->context);
//         context->setTrapFlag();
//         return true;
//      }
//   }
//
//   return false;
//}
//
//bool Win32BreakpointContext :: processBreakpoint(Win32ThreadContext* context)
//{
//   if (breakpoints.exist(getIP(context->context) - 1)) {
//      context->breakpoint.next = getIP(context->context) - 1;
//
//      context->setIP(context->breakpoint.next);
//      char substitute = breakpoints.get(context->breakpoint.next);
//      context->writeDump(context->breakpoint.next, &substitute, 1);
//
//      context->breakpoint.software = true;
//
//      return true;
//   }
//   else return false;
//
//}
//
//void Win32BreakpointContext :: setHardwareBreakpoint(addr_t address, Win32ThreadContext* context, bool withStackControl)
//{
//   if (address == getIP(context->context)) {
//      context->setTrapFlag();
//      context->breakpoint.next = address;
//   }
//   else {
//      context->setHardwareBreakpoint(address);
//   }
//
//   if (withStackControl) {
//      context->breakpoint.stackLevel = getBP(context->context);
//   }
//   else context->breakpoint.stackLevel = 0;
//}
//
//
//void Win32BreakpointContext :: clear()
//{
//   breakpoints.clear();
//}
//
//// --- Win32DebugProcess::ConsoleHelper ---
//
//void Win32DebugProcess::ConsoleHelper :: printText(const char* s)
//{
//   HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);
//
//   WriteConsoleA(output_handle, s, getlength_pos(s), 0, 0);
//}
//
//void Win32DebugProcess::ConsoleHelper :: waitForAnyKey()
//{
//   HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE);
//   INPUT_RECORD input_record;
//   DWORD input_length = 1;
//   DWORD events_read = 0;
//
//   while (true) {
//      ReadConsoleInput(input_handle, &input_record, input_length, &events_read);
//
//      if (input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown) {
//         return;
//      }
//   }
//}
//
//// --- Win32DebugProcess ---
//
//Win32DebugProcess :: Win32DebugProcess()
//   : _threads(nullptr), steps(nullptr)
//{
//   reset();
//   needToFreeConsole = false;
//}
//
//inline bool isIncluded(path_t paths, path_t path)
//{
//   pos_t length = path.length_pos();
//   pos_t index = paths.findStr(path);
//
//   return (index != NOTFOUND_POS && (paths[index + length] == ';' || paths[index + length] == 0 || paths[index + length] == '\\'));
//}
//
//bool Win32DebugProcess :: startProcess(const wchar_t* exePath, const wchar_t* cmdLine, const wchar_t* appPath,
//   StartUpSettings& startUpSettings)
//{
//   DynamicString<path_c> pathsEnv;
//
//   PROCESS_INFORMATION pi = { nullptr, nullptr, 0, 0 };
//   STARTUPINFO         si;
//   PathString          currentPath;
//   DWORD               flags = DEBUG_PROCESS;
//
//   currentPath.copySubPath(exePath, false);
//
//   memset(&si, 0, sizeof(si));
//
//   si.dwFlags = STARTF_USESHOWWINDOW;
//   si.wShowWindow = SW_SHOWNORMAL;
//
//   if (startUpSettings.withExplicitConsole) {
//      AllocConsole();
//
//      needToFreeConsole = true;
//   }
//   else flags |= CREATE_NEW_CONSOLE;
//
//   pos_t trimPos = INVALID_POS;
//   if (startUpSettings.includeAppPath2Paths) {
//      flags |= CREATE_UNICODE_ENVIRONMENT;
//
//      pathsEnv.allocate(4096);
//
//      int dwRet = GetEnvironmentVariable(_T("PATH"), (LPWSTR)pathsEnv.str(), 4096);
//      if (dwRet && !isIncluded(pathsEnv.str(), appPath)) {
//         trimPos = pathsEnv.length_pos();
//
//         if (!pathsEnv.empty() && pathsEnv[pathsEnv.length() - 1] != ';')
//            pathsEnv.append(';');
//         pathsEnv.append(appPath);
//
//         SetEnvironmentVariable(_T("PATH"), pathsEnv.str());
//      }
//   }
//
//   bool retVal = CreateProcess(
//      exePath,
//      (wchar_t*)cmdLine,
//      nullptr,
//      nullptr,
//      FALSE,
//      flags,
//      nullptr,
//      currentPath.str(), &si, &pi);
//
//   if (trimPos != NOTFOUND_POS) {
//      // rolling back changes to ENVIRONENT if required
//      pathsEnv.trim(trimPos);
//
//      SetEnvironmentVariable(_T("PATH"), pathsEnv.str());
//   }
//
//   if (!retVal) {
//      return false;
//   }
//
//   dwDebugeeProcessId = pi.dwProcessId;
//
//   if (pi.hProcess)
//      CloseHandle(pi.hProcess);
//
//   if (pi.hThread)
//      CloseHandle(pi.hThread);
//
//   started = true;
//   //exception.code = 0;
//   needToHandle = false;
//
//   return true;
//}
//
//bool Win32DebugProcess :: startProgram(path_t exePath, path_t cmdLine, path_t appPath, StartUpSettings& startUpSettings)
//{
//   if (startProcess(exePath.str(), cmdLine.str(), appPath, startUpSettings)) {
//      processEvent(INFINITE);
//
//      return true;
//   }
//   else return false;
//}
//
//bool Win32DebugProcess :: startThread(DebugControllerBase* controller)
//{
//   HANDLE hThread = CreateThread(nullptr, 4096,
//      (LPTHREAD_START_ROUTINE)debugEventThread,
//      (LPVOID)controller,
//      0, &threadId);
//
//   if (!hThread) {
//      return false;
//   }
//   else ::CloseHandle(hThread);
//
//   return true;
//}
//
//void Win32DebugProcess :: continueProcess()
//{
//   int code = needToHandle ? DBG_EXCEPTION_NOT_HANDLED : DBG_CONTINUE;
//
//   ContinueDebugEvent(dwCurrentProcessId, dwCurrentThreadId, code);
//
//   needToHandle = false;
//}
//
//void Win32DebugProcess :: processEnd()
//{
//   _threads.clear();
//   _current = nullptr;
//   started = false;
//   if (needToFreeConsole) {
//      ConsoleHelper console;
//      console.printText(CONSOLE_OUTPUT_TEXT);
//      console.waitForAnyKey();
//
//      FreeConsole();
//
//      needToFreeConsole = false;
//   }
//}
//
//void Win32DebugProcess :: processEvent(DWORD timeout)
//{
//   DEBUG_EVENT event;
//
//   trapped = false;
//   if (WaitForDebugEvent(&event, timeout)) {
//      dwCurrentThreadId = event.dwThreadId;
//      dwCurrentProcessId = event.dwProcessId;
//
//      switch (event.dwDebugEventCode) {
//         case CREATE_PROCESS_DEBUG_EVENT:
//            _current = new Win32ThreadContext(event.u.CreateProcessInfo.hProcess, event.u.CreateProcessInfo.hThread);
//            _current->refresh();
//
//            _threads.add(dwCurrentThreadId, _current);
//
//            if (dwCurrentProcessId == dwDebugeeProcessId) {
//               baseAddress = (addr_t)event.u.CreateProcessInfo.lpBaseOfImage;
//               _breakpoints.setSoftwareBreakpoints(_current);
//            }
//
//            ::CloseHandle(event.u.CreateProcessInfo.hFile);
//            break;
//         case EXIT_PROCESS_DEBUG_EVENT:
//            _current = _threads.get(dwCurrentThreadId);
//            if (_current) {
//               _current->refresh();
//               //exitCheckPoint = proceedCheckPoint();
//            }
//            processEnd();
//            break;
//         case CREATE_THREAD_DEBUG_EVENT:
//            _current = new Win32ThreadContext((*_threads.start())->hProcess, event.u.CreateThread.hThread);
//            _current->refresh();
//
//            _threads.add(dwCurrentThreadId, _current);
//            break;
//         case EXIT_THREAD_DEBUG_EVENT:
//            _threads.erase(event.dwThreadId);
//            _current = *_threads.start();
//            break;
//         case LOAD_DLL_DEBUG_EVENT:
//            ::CloseHandle(event.u.LoadDll.hFile);
//            break;
//         case UNLOAD_DLL_DEBUG_EVENT:
//            break;
//         case OUTPUT_DEBUG_STRING_EVENT:
//            break;
//         case RIP_EVENT:
//            started = false;
//            break;
//         case EXCEPTION_DEBUG_EVENT:
//            _current = _threads.get(dwCurrentThreadId);
//            if (_current) {
//               _current->refresh();
//               processException(&event.u.Exception);
//               _current->refresh();
//            }
//            break;
//      }
//   }
//}
//
//void Win32DebugProcess :: processException(EXCEPTION_DEBUG_INFO* exception)
//{
//   switch (exception->ExceptionRecord.ExceptionCode) {
//      case EXCEPTION_SINGLE_STEP:
//         if (_breakpoints.processStep(_current, stepMode))
//            break;
//
//         // stop if it is VM Hook mode
//         if (init_breakpoint == INVALID_ADDR) {
//            init_breakpoint = getIP(_current->context);
//            trapped = true;
//         }
//         else if (getIP(_current->context) >= minAddress && getIP(_current->context) <= maxAddress) {
//            processStep();
//         }
//         if (!trapped)
//            _current->setTrapFlag();
//
//         break;
//      case EXCEPTION_BREAKPOINT:
//         if (_breakpoints.processBreakpoint(_current)) {
//            _current->state = steps.get(getIP(_current->context));
//            trapped = true;
//            stepMode = false;
//            _current->setTrapFlag();
//         }
//         else if (init_breakpoint != 0 && init_breakpoint != INVALID_ADDR) {
//            trapped = true;
//            init_breakpoint = getIP(_current->context);
//         }
//         break;
//      default:
//         if (exception->dwFirstChance != 0) {
//            needToHandle = true;
//         }
//         else {
//            this->exception.code = exception->ExceptionRecord.ExceptionCode;
//            this->exception.address = (addr_t)exception->ExceptionRecord.ExceptionAddress;
//            TerminateProcess(_current->hProcess, 1);
//         }
//         break;
//   }
//}
//
//void Win32DebugProcess :: processStep()
//{
//   _current->state = steps.get(getIP(_current->context));
//   if (_current->state != nullptr) {
//      trapped = true;
//      stepMode = false;
//      //proceedCheckPoint();
//   }
//}
//
//bool Win32DebugProcess :: proceed(int timeout)
//{
//   processEvent(timeout);
//
//   return !trapped;
//}
//
//void Win32DebugProcess :: resetException()
//{
//   exception.code = 0;
//}
//
//void Win32DebugProcess :: run()
//{
//   continueProcess();
//}
//
//void Win32DebugProcess :: activate()
//{
//   if (started) {
//      EnumWindows(EnumThreadWndProc, dwCurrentThreadId);
//   }
//}
//
//void Win32DebugProcess :: stop()
//{
//   if (!started)
//      return;
//
//   if (_current)
//      ::TerminateProcess(_current->hProcess, 1);
//
//   continueProcess();
//}
//
//void Win32DebugProcess :: reset()
//{
//   trapped = false;
//
//   _threads.clear();
//   _current = nullptr;
//
//   minAddress = INVALID_ADDR;
//   maxAddress = 0;
//   baseAddress = 0;
//
//   dwDebugeeProcessId = 0;
//
//   steps.clear();
//   _breakpoints.clear();
//
//   init_breakpoint = 0;
//   stepMode = false;
//   needToHandle = false;
//   //exitCheckPoint = false;
//
//   //_vmHook = 0;
//}
//
//addr_t Win32DebugProcess :: findEntryPoint(path_t programPath)
//{
//   return PEHelper::findEntryPoint(programPath);
//}
//
//bool Win32DebugProcess :: isInitBreakpoint()
//{
//   return _current ? init_breakpoint == getIP(_current->context) : false;
//}
//
//addr_t Win32DebugProcess :: getBaseAddress()
//{
//   return baseAddress;
//}
//
//bool Win32DebugProcess :: findSignature(StreamReader& reader, char* signature, pos_t length)
//{
//   size_t rdata = 0;
//   if (!PEHelper::seekSection(reader, ".rdata", rdata))
//      return false;
//
//   // load Executable image
//   _current->readDump(rdata + sizeof(addr_t), signature, length);
//
//   return true;
//}
//
//void Win32DebugProcess :: setBreakpoint(addr_t address, bool withStackLevelControl)
//{
//   _breakpoints.setHardwareBreakpoint(address, _current, withStackLevelControl);
//}
//
//void Win32DebugProcess :: addBreakpoint(addr_t address)
//{
//   _breakpoints.addBreakpoint(address, _current, started);
//}
//
//void Win32DebugProcess :: removeBreakpoint(addr_t address)
//{
//   _breakpoints.removeBreakpoint(address, _current, started);
//}
//
//void Win32DebugProcess :: setStepMode()
//{
//   // !! temporal
//   _current->clearHardwareBreakpoint();
//
//   _current->setTrapFlag();
//   stepMode = true;
//}
//
//void Win32DebugProcess :: addStep(addr_t address, void* state)
//{
//   steps.add(address, state);
//   if (address < minAddress)
//      minAddress = address;
//
//   if (address > maxAddress)
//      maxAddress = address;
//}
//
//int Win32DebugProcess :: getDataOffset()
//{
//   return sizeof(addr_t);
//}
//
//void* Win32DebugProcess :: getState()
//{
//   return _current ? _current->state : nullptr;
//}
//
//addr_t Win32DebugProcess :: getClassVMT(addr_t address)
//{
//   addr_t ptr = 0;
//
//   if (_current->readDump(address - elObjectOffset, (char*)&ptr, sizeof(addr_t))) {
//      return ptr;
//   }
//   else return 0;
//}
//
//addr_t Win32DebugProcess :: getStackItemAddress(disp_t disp)
//{
//   return getBP(_current->context) - disp;
//}
//
//addr_t Win32DebugProcess :: getStackItem(int index, disp_t offset)
//{
//   return getMemoryPtr(getStackItemAddress(index * sizeof(addr_t) + offset));
//}
//
//addr_t Win32DebugProcess :: getMemoryPtr(addr_t address)
//{
//   addr_t retPtr = 0;
//
//   if (_current->readDump(address, (char*)&retPtr, sizeof(addr_t))) {
//      return retPtr;
//   }
//   else return 0;
//
//}
//
//unsigned short Win32DebugProcess :: getWORD(addr_t address)
//{
//   unsigned short word = 0;
//
//   if (_current->readDump(address, (char*)&word, 2)) {
//      return word;
//   }
//   else return 0;
//}
//
//unsigned Win32DebugProcess::getDWORD(addr_t address)
//{
//   unsigned int dword = 0;
//
//   if (_current->readDump(address, (char*)&dword, 4)) {
//      return dword;
//   }
//   else return 0;
//}
//
//char Win32DebugProcess::getBYTE(addr_t address)
//{
//   char b = 0;
//
//   if (_current->readDump(address, (char*)&b, 1)) {
//      return b;
//   }
//   else return 0;
//}
//
//unsigned long long Win32DebugProcess :: getQWORD(addr_t address)
//{
//   unsigned long long qword = 0;
//
//   if (_current->readDump(address, (char*)&qword, 8)) {
//      return qword;
//   }
//   else return 0;
//}
//
//double Win32DebugProcess :: getFLOAT64(addr_t address)
//{
//   double number = 0;
//
//   if (_current->readDump(address, (char*)&number, 8)) {
//      return number;
//   }
//   else return 0;
//
//}
//
//ref_t Win32DebugProcess :: getClassFlags(addr_t vmtAddress)
//{
//   ref_t flags = 0;
//   if (_current->readDump(vmtAddress - elVMTFlagOffset, (char*)&flags, sizeof(flags))) {
//      return flags;
//   }
//   else return 0;
//
//}
//
//addr_t Win32DebugProcess :: getField(addr_t address, int index)
//{
//   disp_t offset = index * sizeof(addr_t);
//
//   return getMemoryPtr(address + offset);
//}
//
//addr_t Win32DebugProcess :: getFieldAddress(addr_t address, disp_t disp)
//{
//   return address + disp;
//}
//
//size_t Win32DebugProcess :: getArrayLength(addr_t address)
//{
//   ObjectHeader header;
//   if (readDump(address - elObjectOffset, (char*)&header, elObjectOffset)) {
//      return header.size & ~elStructMask;
//   }
//
//   return 0;
//}
