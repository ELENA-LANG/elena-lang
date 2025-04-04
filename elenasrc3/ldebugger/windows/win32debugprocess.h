//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Win32 Debugger class header
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WIN32DEBUGPROCESS_H
#define WIN32DEBUGPROCESS_H

#include <windows.h>

#include "ldbg_common.h"

namespace elena_lang
{
   // --- Win32DebugProcessException ---
   struct Win32DebugProcessException : public DebugProcessException
   {         
   };

   // --- Win32ThreadBreakpoint ---
   struct Win32ThreadBreakpoint
   {
      bool   software;
      bool   hardware;
      addr_t next;
      addr_t stackLevel;

      void clearSoftware()
      {
         software = false;
         next = 0;
      }

      Win32ThreadBreakpoint()
      {
         hardware = software = false;
         stackLevel = next = 0;
      }
   };

   // --- Win32ThreadContext ---
   struct Win32ThreadContext
   {
      friend class Win32DebugProcess;
      friend struct Win32BreakpointContext;
   
      void*   state;
      HANDLE  hProcess;
      HANDLE  hThread;
      CONTEXT context;
   
   public:
      Win32ThreadBreakpoint breakpoint;
   
      bool readDump(addr_t address, char* dump, size_t length);
      void writeDump(addr_t address, char* dump, size_t length);
   
      void refresh();
   
      void setTrapFlag();
      void resetTrapFlag();
      void setHardwareBreakpoint(addr_t breakpoint);
      void clearHardwareBreakpoint();
   
      unsigned char setSoftwareBreakpoint(addr_t breakpoint);
      void clearSoftwareBreakpoint(addr_t breakpoint, unsigned char substitute);
   
      void setIP(addr_t address);
   
      Win32ThreadContext(HANDLE hProcess, HANDLE hThread);
   };
   
   // --- BreakpointContext ---
   struct Win32BreakpointContext
   {
      Map<size_t, char> breakpoints;
   
      void addBreakpoint(addr_t address, Win32ThreadContext* context, bool started);
      void removeBreakpoint(addr_t address, Win32ThreadContext* context, bool started);
      void setSoftwareBreakpoints(Win32ThreadContext* context);
      void setHardwareBreakpoint(addr_t address, Win32ThreadContext* context, bool withStackLevelControl);
   
      bool processStep(Win32ThreadContext* context, bool stepMode);
      bool processBreakpoint(Win32ThreadContext* context);
   
      void clear();
   
      Win32BreakpointContext();
   };

   class Win32DebugProcess
   {
      typedef Map<int, Win32ThreadContext*, nullptr, nullptr, freeobj>  ThreadContextes;
      typedef MemoryMap<addr_t, void*, Map_StoreAddr, Map_GetAddr>      StepMap;

      class ConsoleHelper
      {
      public:
         void printText(const char* s);
         void waitForAnyKey();
      };
      
      const char*                _endingMessage;

      bool                       _needToHandle;
      bool                       _needToFreeConsole;

      bool                       _started;
      bool                       _trapped;
      bool                       _stepMode;

      DWORD                      _dwCurrentProcessId;
      DWORD                      _dwCurrentThreadId;
      DWORD                      _dwDebugeeProcessId;

      addr_t                     _minAddress, _maxAddress;
      addr_t                     _baseAddress;

      Win32DebugProcessException _exception;
      Win32ThreadContext*        _current;
      Win32BreakpointContext     _breakpoints;
      ThreadContextes            _threads;
      StepMap                    _steps;

      void processStep();
      void processException(EXCEPTION_DEBUG_INFO* exception);
      void processEnd();

   public:
      bool isTrapped()
      {
         return _trapped;
      }

      void reset();

      void continueProcess();
      void processEvent(DWORD timeout);

      bool startProcess(const wchar_t* exePath, const wchar_t* cmdLine, const wchar_t* appPath,
         bool includeAppPath2Paths, bool withExplicitConsole);

      Win32DebugProcess(const char* endingMessage);
   };
}

#endif