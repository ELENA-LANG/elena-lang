//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the Linux Debugger Adapter class header
//                                             (C)2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LNXDEBUGADAPTER_H
#define LNXDEBUGADAPTER_H

#include "idecommon.h"

#include <unistd.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <pthread.h>

namespace elena_lang
{
   class DebugProcessController;
   class BreakpointController;

   struct TempBreakpoint
   {
      enum class Mode
      {
         None     = 0,
         Pending  = 1,
         Software = 2,
         Reset    = 3,
      };

      Mode   mode;
      addr_t address;
      char   substitute;

      bool isAssigned() const { return mode != Mode::None; }

      bool isPending() const { return mode == Mode::Pending; }

      void reset()
      {
         mode = Mode::None;
      }

      TempBreakpoint()
         : mode(Mode::None), address(0), substitute(0)
      {
      }
      TempBreakpoint(addr_t address)
         : mode(Mode::None), address(address), substitute(0)
      {
      }
      TempBreakpoint(addr_t address, Mode mode)
         : mode(mode), address(address), substitute(0)
      {
      }
   };

   struct ThreadContext
   {
      friend class DebugProcessController;
      friend struct BreakpointController;

   protected:
      void*                   _state;
      pid_t                   _threadId;

      struct user_regs_struct _context;
      bool                    _stepMode;

      TempBreakpoint          _resetBreakpoint;

   public:
      bool readDump(addr_t address, char* dump, size_t length);
      void writeDump(addr_t address, char* dump, size_t length);

      void refresh();

      void setTrapFlag();
      void resetTrapFlag();

      char setSoftwareBreakpoint(addr_t breakpoint);
      void clearSoftwareBreakpoint(addr_t breakpoint, char substitute);

      addr_t IP();
      void setIP(addr_t address);

      addr_t BP();

      bool checkStepRange(addr_t minAddress, addr_t maxAddress);

      ThreadContext(pid_t pid);
   };

   // --- BreakpointContext ---

   typedef CachedList<TempBreakpoint, 5> TempBreakpoints;
   typedef List<addr_t>                  BreakpointList;
   typedef Map<addr_t, char>             BreakpointMap;

   struct BreakpointController
   {
      bool              pendingAvailable;

      TempBreakpoints   tempBreakpoints;

      BreakpointList    newBreakpoints;  // list of breakpoints we need to set
      BreakpointMap     breakpoints;

      bool applyPendingBreakpoints(ThreadContext* context);

      void setTempBreakpoint(addr_t address, ThreadContext* context, bool pendingMode);
      bool clearTempBreakpoint(addr_t address, ThreadContext* context);

      bool processStep(ThreadContext* context);

      void addBreakpoint(addr_t address, ThreadContext* context, bool pending);
      void removeBreakpoint(addr_t address, ThreadContext* context, bool started);
      bool setSoftwareBreakpoints(ThreadContext* context);

      bool processBreakpoint(ThreadContext* context);

      void clear();

      BreakpointController();
   };

   // --- ProcessException ---
   struct ProcessException
   {
      int    code;
      addr_t address;

      //const char* Text();

      ProcessException()
      {
         code = 0;
      }
   };

   // --- DebugProcessController ---
   class DebugProcessController
   {
      typedef Map<pid_t, ThreadContext*, nullptr, nullptr, freeobj>  ThreadContextes;
      typedef MemoryMap<addr_t, void*, Map_StoreAddr, Map_GetAddr>      StepMap;

      ProcessException     _exception;

      bool                 _started;
      bool                 _trapped;

      pid_t                _traceeId;
      pid_t                _currentId;

      ThreadContextes      _threads;
      StepMap              _steps;

      ThreadContext*       _current;
      BreakpointController _breakpoints;

      addr_t               _init_breakpoint;
      addr_t               _minAddress, _maxAddress;

   public:
      void initHook() { _init_breakpoint = INVALID_ADDR; }

      bool isStarted() { return _started; }
      bool isTrapped() const { return _trapped; }
      bool isHooked() const { return _current != nullptr && _init_breakpoint == INVALID_ADDR; }

      bool isInitBreakpoint();

      void setInitBreakpoint();

      void setStepMode();

      bool startProcess(const char* exePath, const char* cmdLine, const char* appPath);
      void continueProcess();
      void stop();
      void reset();

      bool findSignature(StreamReader& reader, char* signature, pos_t length);

      addr_t getBaseAddress();
      void* getState();
      void* retrieveState(addr_t address)
      {
         return _steps.get(address);
      }

      addr_t getStackFrame();
      addr_t getIP();
      addr_t getMemoryPtr(addr_t address);
      ref_t getMemoryRef(addr_t address);
      template<class T> bool read(addr_t address, T& value)
      {
         return _current && _current->readDump(address, (char*)&value, sizeof(T));
      }
      bool read(addr_t address, char* output, pos_t length)
      {
         return _current && _current->readDump(address, output, length);
      }

      void processEvent();
      void processSignal(int signal);
      void processStep();

      ProcessException* getException()
      {
         return _exception.code == 0 ? nullptr : &_exception;
      }
      void resetException();

      void addBreakpoint(addr_t address);
      void removeBreakpoint(addr_t address);

      void setBreakpoint(addr_t address);

      void addStep(addr_t address, void* state);

      DebugProcessController();
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
      pthread_t               _threadId;

      DebugEventManager       _events;
      DebugProcessController  _process;

      DebugProcessException   _exception;

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

      bool isStarted() override;
      bool isTrapped() override;
      bool isInitBreakpoint() override;

      void initHook() override
      {
         _process.initHook();
      }

      int getDataOffset() override;

      addr_t getBaseAddress() override;
      void* getState() override;

      void* retrieveState(addr_t address) override;

      //virtual addr_t getMemoryPtr(addr_t address) = 0;

      addr_t getFrame() override;
      addr_t getIP() override;

      addr_t getStackItem(int index, disp_t offset = 0) override;
      addr_t getStackItemAddress(disp_t disp) override;

      addr_t getField(addr_t address, int index) override;
      addr_t getFieldAddress(addr_t address, disp_t disp) override;

      addr_t getClassVMT(addr_t address) override;
      ref_t getClassFlags(addr_t vmtAddress) override;

      size_t getArrayLength(addr_t address) override;

      char getBYTE(addr_t address) override;
      unsigned short getWORD(addr_t address) override;
      unsigned int getDWORD(addr_t address) override;
      unsigned long long getQWORD(addr_t address) override;
      double getFLOAT64(addr_t address) override;

      void addBreakpoint(addr_t address) override;
      void removeBreakpoint(addr_t address) override;

      addr_t findEntryPoint(path_t programPath) override;
      bool findSignature(StreamReader& reader, char* signature, pos_t length) override;

      DebugProcessException* Exception() override;
      void resetException() override;

      void activate() override;
      void run() override;
      bool proceed(int timeout) override;
      void stop()  override;
      void reset() override;

      void setStepMode() override;

      bool readDump(addr_t address, char* s, pos_t length) override;

      void addStep(addr_t address, void* current) override;

      void setBreakpoint(addr_t address, bool withStackLevelControl) override;

      bool startThread(DebugControllerBase* controller) override;

      bool startProgram(path_t exePath, path_t cmdLine, path_t appPath, StartUpSettings& startUpSettings) override;

      LnxDebugAdapter();
   };
}

#endif // LNXDEBUGADAPTER_H
