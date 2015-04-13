//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the Debugger class and its helpers header
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef gtkdebuggerH
#define gtkdebuggerH

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
//   //HANDLE _events[MAX_DEBUG_EVENT];

public:
   void init();
   void resetEvent(int event);
   void setEvent(int event);
   int  waitForAnyEvent();
   bool waitForEvent(int event, int timeout);
   void close();

   DebugEventManager()
   {
//      for (int i = 0 ; i < MAX_DEBUG_EVENT ; i++)
//         _events[i] = NULL;
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

   void clearSoftware()
   {
      software = false;
      next = 0;
   }

   ThreadBreakpoint()
   {
      hardware = software = false;
      stackLevel = next = 0;
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

struct ThreadContext
{
   friend class Debugger;
   friend struct BreakpointContext;

protected:
   void*   state;
//   HANDLE  hProcess;
//   HANDLE  hThread;
//   CONTEXT context;

public:
   ThreadBreakpoint breakpoint;

   bool atCheckPoint;
   bool checkFailed;

   void* State() const { return state; }
   size_t EIP() const { return /*context.Eip*/0; } // !! temporal
   size_t Frame() { return /*context.Ebp - offset * */4; } // !! temporal
   size_t Local(int offset) { return /*context.Ebp - offset * */4; } // !! temporal
   size_t Current(int offset) { return /*context.Esp + offset * */4; }// !! temporal
   size_t ClassVMT(size_t address);
   size_t VMTFlags(size_t address);
   size_t ObjectPtr(size_t address);
   size_t LocalPtr(int offset) { return ObjectPtr(Local(offset)); }
   size_t CurrentPtr(int offset) { return ObjectPtr(Current(offset)); }

   void readDump(size_t address, char* dump, size_t length);
   void writeDump(size_t address, char* dump, size_t length);

   size_t readDWord(size_t address) { return 0; } // !! temporal
   size_t readWord(size_t address) { return 0; } // !! temporal

   void refresh();

   void setCheckPoint();
   void setTrapFlag();
   void resetTrapFlag();
   void setHardwareBreakpoint(size_t breakpoint);
   unsigned char setSoftwareBreakpoint(size_t breakpoint);
   void setEIP(size_t address);

   void clearHardwareBreakpoint();
   void clearSoftwareBreakpoint(size_t breakpoint, char substitute);

//   ThreadContext(HANDLE hProcess, HANDLE hThread);
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

   BreakpointContext();
};

// --- Debugger ---

class Debugger
{
//   typedef Map<int, ThreadContext*> ThreadContextes;
//
//   DWORD             threadId;
//
   bool              started;
   bool              trapped;
//   bool              stepMode;
//   bool              needToHandle;
//   bool              exitCheckPoint;
//
//   BreakpointContext breakpoints;
//
//   ThreadContextes   threads;
//   ThreadContext*    current;
//
//   DWORD             dwCurrentProcessId;
//   DWORD             dwCurrentThreadId;
//
//   size_t            minAddress, maxAddress;
////   size_t            vmhookAddress; // =0 - hook is turned off, =-1 : waiting for initializing, >0 hook address
//
//   MemoryMap<int, void*> steps;
//
//   ProcessException exception;
//
//   bool startProcess(const TCHAR* exePath, const TCHAR* cmdLine);
//   void processEvent(size_t timeout);
//   void processException(EXCEPTION_DEBUG_INFO* exception);
//   void continueProcess();
//
//   void processStep();

public:
   bool isStarted() const { return started; }
   bool isTrapped() const { return trapped; }
   // !! temporal
   bool isInitBreakpoint() const { return /*vmhookAddress == current->context.Eip;*/ false; }

   ThreadContext* Context() { return /*current*/NULL; }
   ProcessException* Exception() { return /*exception.code == 0 ? */NULL/* : &exception*/; } // !! temporal

   void resetException() { /*exception.code = 0;*/ }

   void addStep(size_t address, void* state);

   void addBreakpoint(size_t address);
   void removeBreakpoint(size_t address);
   void clearBreakpoints();

   void setStepMode();
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

   // !! temporal
   void initHook() { /*vmhookAddress = -1;*/ }
   bool initDebugInfo(bool standalone, StreamReader& reader, size_t& debugInfoPtr);

   size_t findEntryPoint(const char* programPath);
   bool findSignature(char* signature);

   Debugger();
};

} // _ELENA_

#endif // gtkdebuggerH
