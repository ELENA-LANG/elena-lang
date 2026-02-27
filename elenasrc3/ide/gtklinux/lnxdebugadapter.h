//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Linux Debugger Adapter class header
//                                             (C)2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LNXDEBUGADAPTER_H
#define LNXDEBUGADAPTER_H

namespace elena_lang
{
   class DebugProcess;

   struct ThreadContext
   {
/*      friend class DebugProcess;
      friend struct BreakpointContext;

   protected:
      Debugger* debugger;
      void*     state;
      pid_t     threadId;

      struct user_regs_struct context;

      void set_breakpoint_addr(void *addr, int n);
*/
   public:
/*      ThreadBreakpoint breakpoint;

      bool atCheckPoint;
      bool checkFailed;

      void* State() const { return state; }
      size_t IP() { return context.eip; }
      size_t Frame() { return context.ebp; }
      size_t Local(int offset) { return context.ebp - offset * 4; }
      size_t Current(int offset) { return context.esp + offset * 4; }
      size_t ClassVMT(size_t address);
      size_t VMTFlags(size_t address);
      size_t ObjectPtr(size_t address);
      size_t LocalPtr(int offset) { return ObjectPtr(Local(offset)); }
      size_t CurrentPtr(int offset) { return ObjectPtr(Current(offset)); }

      bool readDump(size_t address, char* dump, size_t length);
      void writeDump(size_t address, char* dump, size_t length);

      size_t readDWord(size_t address)
      {
         size_t word = 0;
         readDump(address, (char*)&word, 4);

         return word;
      }
   
      size_t readWord(size_t address)
      {
         size_t word = 0;
         readDump(address, (char*)&word, 2);

         return word;
      }
   
      size_t readByte(size_t address)
      {
         size_t word = 0;
         readDump(address, (char*)&word, 1);

         return word;
      }

      void writeDWord(size_t address, size_t word)
      {
         writeDump(address, (char*)&word, 4);
      }

      void refresh();

      void setCheckPoint();
      void setHardwareBreakpoint(size_t breakpoint);
      unsigned char setSoftwareBreakpoint(size_t breakpoint);
      void setEIP(size_t address);

      void clearHardwareBreakpoint();
      void clearSoftwareBreakpoint(size_t breakpoint, char substitute);

      void setTrapFlag();
      void resetTrapFlag();
*/
      ThreadContext(/*Debugger* debugger, pid_t pid*/);
   };

   // --- BreakpointContext ---

   struct BreakpointContext
   {
/*      Map<size_t, char> breakpoints;

      void addBreakpoint(size_t address, ThreadContext* context, bool started);
      void removeBreakpoint(size_t address, ThreadContext* context, bool started);
      void setSoftwareBreakpoints(ThreadContext* context);
      void setHardwareBreakpoint(size_t address, ThreadContext* context, bool withStackLevelControl);

      bool processStep(ThreadContext* context, bool stepMode);
      bool processBreakpoint(ThreadContext* context);

      void clear();
      bool applyPendingBreakpoints(ThreadContext* context);
*/   
      BreakpointContext();
   };

   // --- ProcessException ---
   struct ProcessException
   {
      int code;
      int address;
   
      const char* Text();
   
      ProcessException()
      {
         code = 0;
      }
   };

   // --- DebugProcess ---
   class DebugProcess
   {
      typedef Map<pid_t, ThreadContext*> ThreadContextes;

      ProcessException exception;

      bool              started;

      pid_t             traceeId;
      pid_t             currentId;

      ThreadContextes   threads;
      ThreadContext*    current;

   public:
      bool startProcess();

      void processEvent(/*DWORD timeout*/);

      DebugProcess();
   };

   // --- DebugEventManager ---
   class DebugEventManager
   {
      int             _flag;
      pthread_cond_t  _event;
      pthread_mutex_t _lock;
   
   public:
      void init();
      void resetEvent(int event);
      void setEvent(int event);
      int  waitForAnyEvent();
      bool waitForEvent(int event, int timeout);
      void close();
   
      DebugEventManager()
      {
         _flag = 0;
      }
      ~DebugEventManager()
      {
         close();
      }
   };

   // --- LnxDebugAdapter ---
   class LnxDebugAdapter : public IDEDebugProcessBase
   {
      DebugEventManager _events;
      DebugProcess      _debugProcess;

   public:
      void initEvents() override
      {
         _events.init();
         _events.setEvent(DEBUG_SUSPEND);
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

      bool startThread(DebugControllerBase* controller) override;

      bool startProgram(path_t exePath, path_t cmdLine, path_t appPath, StartUpSettings& startUpSettings) override;

      LnxDebugAdapter();
   };
}

#endif // LNXDEBUGADAPTER_H
