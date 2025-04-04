//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Win32 Debugger Adapter class header
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WIN32DEBUGADAPTER_H
#define WIN32DEBUGADAPTER_H

#include "idecommon.h"
#include "windows/win32debugprocess.h"

namespace elena_lang
{
//   // --- DebugEventManager ---
//   typedef EventManager<int, MAX_DEBUG_EVENT> DebugEventManager;
//
//   class Win32DebugProcess;
//   struct Win32BreakpointContext;

   // --- Win32DebugAdapter ---
   class Win32DebugAdapter : public DebugProcessBase
   {
      Win32DebugProcess _debugProcess;

//   protected:
//      DebugEventManager          _events;
//
//      addr_t                     init_breakpoint;
//
//      DWORD                      threadId;
//      
//      Win32DebugProcessException exception;
//
//      bool startProcess(const wchar_t* exePath, const wchar_t* cmdLine, const wchar_t* appPath,
//         StartUpSettings& startUpSettings);
//
//      void continueProcess();
//      void processEvent(DWORD timeout);

   public:
      void initEvents() override
      {
//         _events.init(DEBUG_ACTIVE);
      }
      void setEvent(int event) override
      {
//         _events.setEvent(event);
      }
      void resetEvent(int event) override
      {
//         _events.resetEvent(event);
      }
      int waitForAnyEvent() override
      {
//         return _events.waitForAnyEvent();

         return 0; // !! temporal
      }
      bool waitForEvent(int event, int timeout) override
      {
//         return _events.waitForEvent(event, timeout);

         return false; // !! temporal
      }
      void clearEvents() override
      {
//         _events.close();
      }

      void resetException() override;

      DebugProcessException* Exception() override
      {
//         return exception.code == 0 ? nullptr : &exception;

         return nullptr; // !! temporal
      }

      bool isStarted() override
      {
         //return started;

         return false; // !! temporal
      }

      bool isTrapped() override
      {
         //return trapped;

         return false; // !! temporal
      }

      bool isInitBreakpoint() override;

      void initHook() override { /*init_breakpoint = INVALID_ADDR;*/ }

      addr_t findEntryPoint(path_t programPath) override;

      bool findSignature(StreamReader& reader, char* signature, pos_t length) override;

      bool startProgram(path_t exePath, path_t cmdLine, path_t appPath, StartUpSettings& startUpSettings) override;

      bool proceed(int timeout) override;
      void run() override;
      void stop() override;
      void reset() override;
      void activate() override;

      void setStepMode() override;

      bool startThread(DebugControllerBase* controller) override;

      int getDataOffset() override;

      addr_t getBaseAddress() override;
      void* getState() override;

      ref_t getClassFlags(addr_t vmtAddress) override;

      addr_t getClassVMT(addr_t address) override;
      addr_t getStackItem(int index, disp_t offset = 0) override;
      addr_t getStackItemAddress(disp_t disp) override;

      addr_t getField(addr_t address, int index) override;
      addr_t getFieldAddress(addr_t address, disp_t disp) override;

      addr_t getMemoryPtr(addr_t address) override;
      char getBYTE(addr_t address) override;
      unsigned short getWORD(addr_t address) override;
      unsigned getDWORD(addr_t address) override;
      unsigned long long getQWORD(addr_t address) override;
      double getFLOAT64(addr_t address) override;

      size_t getArrayLength(addr_t address) override;

      void setBreakpoint(addr_t address, bool withStackLevelControl) override;
      void addBreakpoint(addr_t address) override;
      void removeBreakpoint(addr_t address) override;

      void addStep(addr_t address, void* current) override;

      bool readDump(addr_t address, char* s, pos_t length) override
      {
         //return _current->readDump(address, s, length);

         return false; // !! temporal
      }

      Win32DebugAdapter();
   };
}

#endif // WIN32DEBUGADAPTER_H
