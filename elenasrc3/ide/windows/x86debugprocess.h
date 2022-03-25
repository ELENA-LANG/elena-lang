//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Debugger class and its helpers header
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef X86DEBUGPROCESS_H
#define X86DEBUGPROCESS_H

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

   // --- x86DebugProcess ---
   class x86DebugProcess : public DebugProcessBase
   {
   protected:
      DebugEventManager _events;

      DWORD             threadId;
      bool              started;
      bool              trapped;
      bool              needToHandle;

      DWORD             dwCurrentProcessId;
      DWORD             dwCurrentThreadId;

      void continueProcess();
      void processEvent(size_t timeout);

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
         return nullptr;
      }

      bool isStarted() override
      {
         return started;
      }

      bool startProgram(const wchar_t* exePath, const wchar_t* cmdLine) override;

      bool proceed(int timeout) override;
      void run() override;
      void stop() override;
      void reset() override;
      void activate() override;

      bool startThread(DebugControllerBase* controller) override;

      x86DebugProcess();
   };

}

#endif
