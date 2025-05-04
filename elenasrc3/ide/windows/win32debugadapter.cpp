//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Win32 Debugger adapter class implementation
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "common.h"
//---------------------------------------------------------------------------
#include "windows/win32debugadapter.h"

#ifdef _MSC_VER

#include <tchar.h>

#endif

#include "eng/messages.h"

using namespace elena_lang;

// --- main thread that is the debugger residing over a debuggee ---

BOOL WINAPI debugEventThread(DebugControllerBase* controller)
{
   controller->debugThread();

   ExitThread(TRUE);

   return TRUE;
}

// --- Win32DebugProcess ---

Win32DebugAdapter :: Win32DebugAdapter()
   : _debugProcess(CONSOLE_OUTPUT_TEXT), _threadId(NULL)
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
   HANDLE hThread = CreateThread(nullptr, 4096,
      (LPTHREAD_START_ROUTINE)debugEventThread,
      (LPVOID)controller,
      0, &_threadId);

   if (!hThread) {
      return false;
   }
   else ::CloseHandle(hThread);

   return true;
}

bool Win32DebugAdapter :: proceed(int timeout)
{
   _debugProcess.processEvent(timeout);

   return !_debugProcess.isTrapped();
}

void Win32DebugAdapter :: resetException()
{
   _exception.address = 0;
   _exception.code = 0;

   _debugProcess.resetException();
}

void Win32DebugAdapter :: run()
{
   _debugProcess.continueProcess();
}

void Win32DebugAdapter :: activate()
{
   _debugProcess.activateWindow();
}

void Win32DebugAdapter :: stop()
{
   _debugProcess.stop();
}

void Win32DebugAdapter :: reset()
{
   _debugProcess.reset();
}

addr_t Win32DebugAdapter :: findEntryPoint(path_t programPath)
{
   return _debugProcess.findEntryPoint(programPath);
}

bool Win32DebugAdapter :: isInitBreakpoint()
{
   return _debugProcess.isInitBreakpoint();
}

addr_t Win32DebugAdapter :: getBaseAddress()
{
   return _debugProcess.getBaseAddress();
}

bool Win32DebugAdapter :: findSignature(StreamReader& reader, char* signature, pos_t length)
{
   return _debugProcess.findSignature(reader, signature, length);
}

void Win32DebugAdapter :: setBreakpoint(addr_t address, bool withStackLevelControl)
{
   _debugProcess.setBreakpoint(address, withStackLevelControl);
}

void Win32DebugAdapter :: addBreakpoint(addr_t address)
{
   _debugProcess.addBreakpoint(address);
}

void Win32DebugAdapter :: removeBreakpoint(addr_t address)
{
   _debugProcess.removeBreakpoint(address);
}

void Win32DebugAdapter :: setStepMode()
{
   _debugProcess.setStepMode();
}

void Win32DebugAdapter :: addStep(addr_t address, void* state)
{
   _debugProcess.addStep(address, state);
}

int Win32DebugAdapter :: getDataOffset()
{
   return sizeof(addr_t);
}

void* Win32DebugAdapter :: getState()
{
   return _debugProcess.getState();
}

addr_t Win32DebugAdapter :: getClassVMT(addr_t address)
{
   return _debugProcess.getClassVMT(address);
}

addr_t Win32DebugAdapter :: getStackItemAddress(disp_t disp)
{
   return _debugProcess.getStackItemAddress(disp);
}

addr_t Win32DebugAdapter :: getStackItem(int index, disp_t offset)
{
   return _debugProcess.getStackItem(index, offset);
}

unsigned short Win32DebugAdapter :: getWORD(addr_t address)
{
   return _debugProcess.getWORD(address);
}

unsigned Win32DebugAdapter :: getDWORD(addr_t address)
{
   return _debugProcess.getDWORD(address);
}

char Win32DebugAdapter :: getBYTE(addr_t address)
{
   return _debugProcess.getBYTE(address);
}

unsigned long long Win32DebugAdapter :: getQWORD(addr_t address)
{
   return _debugProcess.getQWORD(address);
}

double Win32DebugAdapter :: getFLOAT64(addr_t address)
{
   return _debugProcess.getFLOAT64(address);
}

ref_t Win32DebugAdapter :: getClassFlags(addr_t vmtAddress)
{
   return _debugProcess.getClassFlags(vmtAddress);
}

addr_t Win32DebugAdapter :: getField(addr_t address, int index)
{
   return _debugProcess.getField(address, index);
}

addr_t Win32DebugAdapter :: getFieldAddress(addr_t address, disp_t disp)
{
   return _debugProcess.getFieldAddress(address, disp);
}

size_t Win32DebugAdapter :: getArrayLength(addr_t address)
{
   return _debugProcess.getArrayLength(address);
}

bool Win32DebugAdapter :: readDump(addr_t address, char* s, pos_t length)
{
   return _debugProcess.readDump(address, s, length);
}
