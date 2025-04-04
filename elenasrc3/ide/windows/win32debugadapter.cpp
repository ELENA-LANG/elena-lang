//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Win32 Debugger adapter class implementation
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

//#include "common.h"
//---------------------------------------------------------------------------
#include "windows/win32debugadapter.h"

//#include "core.h"
//#include "windows/pehelper.h"

#ifdef _MSC_VER

#include <tchar.h>

#endif

#include "eng/messages.h"

using namespace elena_lang;

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

// --- Win32DebugProcess ---

Win32DebugAdapter :: Win32DebugAdapter()
   : _debugProcess(CONSOLE_OUTPUT_TEXT)
{
}

bool Win32DebugAdapter :: startProgram(path_t exePath, path_t cmdLine, path_t appPath, StartUpSettings& startUpSettings)
{
   if (_debugProcess.startProcess(exePath.str(), cmdLine.str(), appPath.str(),
      startUpSettings.includeAppPath2Paths, startUpSettings.withExplicitConsole))
   {
      _debugProcess.processEvent(INFINITE);

      return true;
   }
   return false;
}

bool Win32DebugAdapter :: startThread(DebugControllerBase* controller)
{
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

   return false; // !! temporal
}

bool Win32DebugAdapter :: proceed(int timeout)
{
   _debugProcess.processEvent(timeout);

   return !_debugProcess.isTrapped();
}

void Win32DebugAdapter :: resetException()
{
//   exception.code = 0;
}

void Win32DebugAdapter :: run()
{
   _debugProcess.continueProcess();
}

void Win32DebugAdapter :: activate()
{
//   if (started) {
//      EnumWindows(EnumThreadWndProc, dwCurrentThreadId);
//   }
}

void Win32DebugAdapter :: stop()
{
//   if (!started)
//      return;
//
//   if (_current)
//      ::TerminateProcess(_current->hProcess, 1);
//
//   continueProcess();
}

void Win32DebugAdapter :: reset()
{
   _debugProcess.reset();
}

addr_t Win32DebugAdapter :: findEntryPoint(path_t programPath)
{
   return 0; // !! temporal

//   return PEHelper::findEntryPoint(programPath);
}

bool Win32DebugAdapter :: isInitBreakpoint()
{
//   return _current ? init_breakpoint == getIP(_current->context) : false;

   return false; // !! temporal
}

addr_t Win32DebugAdapter :: getBaseAddress()
{
//   return baseAddress;


   return 0; // !! temporal
}

bool Win32DebugAdapter :: findSignature(StreamReader& reader, char* signature, pos_t length)
{
//   size_t rdata = 0;
//   if (!PEHelper::seekSection(reader, ".rdata", rdata))
//      return false;
//
//   // load Executable image
//   _current->readDump(rdata + sizeof(addr_t), signature, length);
//
//   return true;

   return false; // !! temporal
}

void Win32DebugAdapter :: setBreakpoint(addr_t address, bool withStackLevelControl)
{
//   _breakpoints.setHardwareBreakpoint(address, _current, withStackLevelControl);
}

void Win32DebugAdapter :: addBreakpoint(addr_t address)
{
//   _breakpoints.addBreakpoint(address, _current, started);
}

void Win32DebugAdapter :: removeBreakpoint(addr_t address)
{
//   _breakpoints.removeBreakpoint(address, _current, started);
}

void Win32DebugAdapter :: setStepMode()
{
//   // !! temporal
//   _current->clearHardwareBreakpoint();
//
//   _current->setTrapFlag();
//   stepMode = true;
}

void Win32DebugAdapter :: addStep(addr_t address, void* state)
{
//   steps.add(address, state);
//   if (address < minAddress)
//      minAddress = address;
//
//   if (address > maxAddress)
//      maxAddress = address;
}

int Win32DebugAdapter :: getDataOffset()
{
//   return sizeof(addr_t);

   return 0; // !! temporal
}

void* Win32DebugAdapter :: getState()
{
//   return _current ? _current->state : nullptr;

   return nullptr;
}

addr_t Win32DebugAdapter :: getClassVMT(addr_t address)
{
//   addr_t ptr = 0;
//
//   if (_current->readDump(address - elObjectOffset, (char*)&ptr, sizeof(addr_t))) {
//      return ptr;
//   }
   /*else */return 0;
}

addr_t Win32DebugAdapter :: getStackItemAddress(disp_t disp)
{
//   return getBP(_current->context) - disp;

   return 0; // !! temporal
}

addr_t Win32DebugAdapter :: getStackItem(int index, disp_t offset)
{
//   return getMemoryPtr(getStackItemAddress(index * sizeof(addr_t) + offset));

   return 0; // !! temporal
}

addr_t Win32DebugAdapter :: getMemoryPtr(addr_t address)
{
//   addr_t retPtr = 0;
//
//   if (_current->readDump(address, (char*)&retPtr, sizeof(addr_t))) {
//      return retPtr;
//   }
   /*else */return 0;
}

unsigned short Win32DebugAdapter :: getWORD(addr_t address)
{
//   unsigned short word = 0;
//
//   if (_current->readDump(address, (char*)&word, 2)) {
//      return word;
//   }
   /*else */return 0;
}

unsigned Win32DebugAdapter :: getDWORD(addr_t address)
{
//   unsigned int dword = 0;
//
//   if (_current->readDump(address, (char*)&dword, 4)) {
//      return dword;
//   }
   /*else */return 0;
}

char Win32DebugAdapter :: getBYTE(addr_t address)
{
//   char b = 0;
//
//   if (_current->readDump(address, (char*)&b, 1)) {
//      return b;
//   }
   /*else */return 0;
}

unsigned long long Win32DebugAdapter :: getQWORD(addr_t address)
{
//   unsigned long long qword = 0;
//
//   if (_current->readDump(address, (char*)&qword, 8)) {
//      return qword;
//   }
   /*else **/return 0;
}

double Win32DebugAdapter :: getFLOAT64(addr_t address)
{
//   double number = 0;
//
//   if (_current->readDump(address, (char*)&number, 8)) {
//      return number;
//   }
   /*else */return 0;

}

ref_t Win32DebugAdapter :: getClassFlags(addr_t vmtAddress)
{
//   ref_t flags = 0;
//   if (_current->readDump(vmtAddress - elVMTFlagOffset, (char*)&flags, sizeof(flags))) {
//      return flags;
//   }
//   else return 0;
//

   return 0; // !! temporal
}

addr_t Win32DebugAdapter :: getField(addr_t address, int index)
{
//   disp_t offset = index * sizeof(addr_t);
//
//   return getMemoryPtr(address + offset);


   return 0; // !! temporal
}

addr_t Win32DebugAdapter :: getFieldAddress(addr_t address, disp_t disp)
{
//   return address + disp;

   return 0; // !! temporal
}

size_t Win32DebugAdapter :: getArrayLength(addr_t address)
{
//   ObjectHeader header;
//   if (readDump(address - elObjectOffset, (char*)&header, elObjectOffset)) {
//      return header.size & ~elStructMask;
//   }

   return 0;
}
