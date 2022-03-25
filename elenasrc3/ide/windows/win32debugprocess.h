//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Win32 Debugger class and its helpers header
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WIN32DEBUGPROCESS_H
#define WIN32DEBUGPROCESS_H

#include "idecommon.h"
#include <windows.h>

namespace elena_lang
{
   // --- DebugEventManager ---
   class DebugEventManager
   {
      HANDLE _events[MAX_DEBUG_EVENT];

   public:
      void init();
      void setEvent(int event);
      void resetEvent(int event);
      int  waitForAnyEvent();
      bool waitForEvent(int event, int timeout);
      void close();

      DebugEventManager()
      {
         for (int i = 0; i < MAX_DEBUG_EVENT; i++)
            _events[i] = nullptr;
      }
      ~DebugEventManager()
      {
         close();
      }
   };

   class Win32DebugProcess;
   struct Win32BreakpointContext;

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
      friend class Win32BreakpointContext;

   protected:
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

      void setIP(addr_t address);

      Win32ThreadContext(HANDLE hProcess, HANDLE hThread);
   };

   // --- BreakpointContext ---
   struct Win32BreakpointContext
   {
      Map<size_t, char> breakpoints;

      //void addBreakpoint(size_t address, Win32ThreadContext* context, bool started);
      //void removeBreakpoint(size_t address, Win32ThreadContext* context, bool started);
      void setSoftwareBreakpoints(Win32ThreadContext* context);
      //void setHardwareBreakpoint(size_t address, Win32ThreadContext* context, bool withStackLevelControl);

      bool processStep(Win32ThreadContext* context, bool stepMode);
      bool processBreakpoint(Win32ThreadContext* context);

      void clear();

      Win32BreakpointContext();
   };

   // --- Win32DebugProcessException ---
   struct Win32DebugProcessException : DebugProcessException
   {
      
   };

   // --- Win32DebugProcess ---
   class Win32DebugProcess : public DebugProcessBase
   {
      typedef Map<int, Win32ThreadContext*, nullptr, nullptr, freeobj>  ThreadContextes;
      typedef MemoryMap<addr_t, void*, Map_StoreAddr, Map_GetAddr>      StepMap;

   protected:
      DebugEventManager          _events;
      Win32BreakpointContext     _breakpoints;
      ThreadContextes            _threads;
      Win32ThreadContext*        _current;

      addr_t                     init_breakpoint;
      addr_t                     minAddress, maxAddress;

      DWORD                      threadId;
      bool                       started;
      bool                       trapped;
      bool                       stepMode;
      bool                       needToHandle;

      DWORD                      dwCurrentProcessId;
      DWORD                      dwCurrentThreadId;

      StepMap                    steps;

      Win32DebugProcessException exception;

      void continueProcess();
      void processEvent(size_t timeout);
      void processException(EXCEPTION_DEBUG_INFO* exception);
      void processStep();

   public:
      void initEvents() override
      {
         _events.init();
      }
      void setEvent(int event) override
      {
         _events.setEvent(event);
      }
      void resetEvent(int event) override
      {
         _events.resetEvent(event);
      }
      int waitForAnyEvent() override
      {
         return _events.waitForAnyEvent();
      }
      bool waitForEvent(int event, int timeout) override
      {
         return _events.waitForEvent(event, timeout);
      }
      void clearEvents() override
      {
         _events.close();
      }

      void resetException() override;

      DebugProcessException* Exception() override
      {
         return exception.code == 0 ? nullptr : &exception;
      }

      bool isStarted() override
      {
         return started;
      }

      void initHook() override { init_breakpoint = INVALID_ADDR; }

      addr_t findEntryPoint(path_t programPath) override;

      bool startProgram(const wchar_t* exePath, const wchar_t* cmdLine) override;

      bool proceed(int timeout) override;
      void run() override;
      void stop() override;
      void reset() override;
      void activate() override;

      bool startThread(DebugControllerBase* controller) override;

      Win32DebugProcess();
   };

}

#endif
