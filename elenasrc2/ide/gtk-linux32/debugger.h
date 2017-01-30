//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the Debugger class and its helpers header
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef gtkdebuggerH
#define gtkdebuggerH

#include <pthread.h>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <sys/reg.h>

#include "../debugging.h"

// --- EventManager ---

#define DEBUG_CLOSE	   0
#define DEBUG_SUSPEND   1
#define DEBUG_RESUME    2
#define DEBUG_ACTIVE	   3
#define MAX_DEBUG_EVENT 4

namespace _ELENA_
{

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

// --- ThreadBreakpoint ---

struct ThreadBreakpoint
{
   bool   software;
   bool   hardware;
   size_t next;
   size_t stackLevel;
   size_t pendingAddress;

   void clearSoftware()
   {
      software = false;
      next = 0;
   }

   ThreadBreakpoint()
   {
      hardware = software = false;
      stackLevel = next = 0;
      pendingAddress = 0;
   }
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

// --- ThreadContext ---

class Debugger;

struct ThreadContext
{
   friend class Debugger;
   friend struct BreakpointContext;

protected:
   Debugger* debugger;
   void*     state;
   pid_t     threadId;

   struct user_regs_struct context;

   void set_breakpoint_addr(void *addr, int n);

public:
   ThreadBreakpoint breakpoint;

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

   ThreadContext(Debugger* debugger, pid_t pid);
};

// --- BreakpointContext ---

struct BreakpointContext
{
   Map<size_t, char> breakpoints;

   void addBreakpoint(size_t address, ThreadContext* context, bool started);
   void removeBreakpoint(size_t address, ThreadContext* context, bool started);
   void setSoftwareBreakpoints(ThreadContext* context);
   void setHardwareBreakpoint(size_t address, ThreadContext* context, bool withStackLevelControl);

   bool processStep(ThreadContext* context, bool stepMode);
   bool processBreakpoint(ThreadContext* context);

   void clear();
   bool applyPendingBreakpoints(ThreadContext* context);

   BreakpointContext();
};

// --- Debugger ---

class Debugger
{
   typedef Map<pid_t, ThreadContext*> ThreadContextes;

   pthread_t         threadId;

   bool              started;
   bool              trapped;
   bool              stepMode;
   bool              exitCheckPoint;
   size_t            init_breakpoint;

   BreakpointContext breakpoints;

   ThreadContextes   threads;
   ThreadContext*    current;

   pid_t             traceeId;
   pid_t             currentId;

   size_t            minAddress, maxAddress;
//   size_t            vmhookAddress; // =0 - hook is turned off, =-1 : waiting for initializing, >0 hook address

   MemoryMap<int, void*> steps;

   ProcessException exception;

   bool startProcess(const char* exePath, const char* cmdLine);
   void processEvent();
   void processSignal(int signal);
   void continueProcess();

   void processStep();

public:
   bool isStarted() const { return started; }
   bool isTrapped() const { return trapped; }
   bool isInitBreakpoint() const { return init_breakpoint == current->context.eip; }

   ThreadContext* Context() { return current; }
   ProcessException* Exception() { return exception.code == 0 ? NULL : &exception; }

   void resetException() { exception.code = 0; }

   void addStep(size_t address, void* state);

   void addBreakpoint(size_t address);
   void removeBreakpoint(size_t address);
   void clearBreakpoints();

   void setStepMode();
   void resetStepMode();
   void setBreakpoint(size_t address, bool withStackLevelControl);
   void setCheckMode();

   bool startThread(_DebugController* controller);

   bool start(const char* exePath, const char* cmdLine);
   void run();
   bool proceed(size_t timeout);
   void stop();

   void processVirtualStep(void* step);
   bool proceedCheckPoint();

   void reset();

   void activate();

   void initHook() { init_breakpoint = -1; }
   bool initDebugInfo(bool standalone, StreamReader& reader, size_t& debugInfoPtr);

   size_t findEntryPoint(const char* programPath);
   bool findSignature(StreamReader& reader, char* signature);

   size_t readRetAddress(_Memory& memory, size_t position)
   {
      return memory[(pos_t)position];
   }

   Debugger();
};

} // _ELENA_

#endif // gtkdebuggerH
