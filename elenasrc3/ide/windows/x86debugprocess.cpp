//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//               
//		This file contains the Debugger class and its helpers implementation
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
//---------------------------------------------------------------------------
#include "windows/x86debugprocess.h"

using namespace elena_lang;

// --- setForegroundWindow() ---
inline void setForegroundWindow(HWND hwnd)
{
   DWORD dwTimeoutMS;
   // Get the current lock timeout.
   ::SystemParametersInfo(0x2000, 0, &dwTimeoutMS, 0);

   // Set the lock timeout to zero
   ::SystemParametersInfo(0x2001, 0, 0, 0);

   // Perform the SetForegroundWindow
   ::SetForegroundWindow(hwnd);

   // Set the timeout back
   ::SystemParametersInfo(0x2001, 0, (LPVOID)dwTimeoutMS, 0);   //HWND hCurrWnd;
}

BOOL CALLBACK EnumThreadWndProc(HWND hwnd, LPARAM lParam)
{
   if (GetWindowThreadProcessId(hwnd, NULL) == (DWORD)lParam) {
      setForegroundWindow(hwnd);

      return FALSE;
   }
   else return TRUE;
}

// --- main thread that is the debugger residing over a debuggee ---

BOOL WINAPI debugEventThread(DebugControllerBase* controller)
{
   controller->debugThread();

   ExitThread(TRUE);

   return TRUE;
}

// --- DebugEventManager ---

void DebugEventManager :: init()
{
   _events[DEBUG_ACTIVE] = CreateEvent(nullptr, TRUE, TRUE, NULL);
   _events[DEBUG_CLOSE] = CreateEvent(nullptr, TRUE, FALSE, NULL);
   _events[DEBUG_SUSPEND] = CreateEvent(nullptr, TRUE, FALSE, NULL);
   _events[DEBUG_RESUME] = CreateEvent(nullptr, TRUE, FALSE, NULL);
}

void DebugEventManager :: setEvent(int event)
{
   SetEvent(_events[event]);
}

void DebugEventManager :: resetEvent(int event)
{
   ResetEvent(_events[event]);
}

bool DebugEventManager :: waitForEvent(int event, int timeout)
{
   return (WaitForSingleObject(_events[event], timeout) == WAIT_OBJECT_0);
}

int DebugEventManager :: waitForAnyEvent()
{
   return WaitForMultipleObjects(MAX_DEBUG_EVENT, _events, FALSE, INFINITE);
}

void DebugEventManager :: close()
{
   for (int i = 0; i < MAX_DEBUG_EVENT; i++) {
      if (_events[i]) {
         CloseHandle(_events[i]);
         _events[i] = nullptr;
      }
   }
}

// --- x86DebugProcess ---

x86DebugProcess :: x86DebugProcess()
{
   threadId = 0;
   trapped = started = false;
   needToHandle = false;
   dwCurrentProcessId = dwCurrentThreadId = 0;
}

bool x86DebugProcess :: startProgram(const wchar_t* exePath, const wchar_t* cmdLine)
{
   PROCESS_INFORMATION pi = { nullptr, nullptr, 0, 0 };
   STARTUPINFO         si;
   PathString          currentPath;

   currentPath.copySubPath(exePath);

   memset(&si, 0, sizeof(si));

   si.dwFlags = STARTF_USESHOWWINDOW;
   si.wShowWindow = SW_SHOWNORMAL;

   if (!CreateProcess(exePath, (wchar_t*)cmdLine, NULL, NULL, FALSE,
      CREATE_NEW_CONSOLE | DEBUG_PROCESS, NULL, currentPath.str(), &si, &pi))
   {
      return false;
   }

   if (pi.hProcess)
      CloseHandle(pi.hProcess);

   if (pi.hThread)
      CloseHandle(pi.hThread);

   started = true;
   //exception.code = 0;
   needToHandle = false;

   return true;
}

bool x86DebugProcess :: startThread(DebugControllerBase* controller)
{
   HANDLE hThread = CreateThread(NULL, 4096,
      (LPTHREAD_START_ROUTINE)debugEventThread,
      (LPVOID)controller,
      0, &threadId);

   if (!hThread) {
      return false;
   }
   else ::CloseHandle(hThread);

   return true;
}

void x86DebugProcess :: continueProcess()
{
   int code = needToHandle ? DBG_EXCEPTION_NOT_HANDLED : DBG_CONTINUE;

   ContinueDebugEvent(dwCurrentProcessId, dwCurrentThreadId, code);

   needToHandle = false;
}

void x86DebugProcess :: processEvent(size_t timeout)
{
   DEBUG_EVENT event;

   trapped = false;
   if (WaitForDebugEvent(&event, timeout)) {
      dwCurrentThreadId = event.dwThreadId;
      dwCurrentProcessId = event.dwProcessId;

      switch (event.dwDebugEventCode) {
         case CREATE_PROCESS_DEBUG_EVENT:
            baseAddress = event.u.CreateProcessInfo.lpBaseOfImage;

            current = new ThreadContext(event.u.CreateProcessInfo.hProcess, event.u.CreateProcessInfo.hThread);
            current->refresh();

            threads.add(dwCurrentThreadId, current);

            breakpoints.setSoftwareBreakpoints(current);

            ::CloseHandle(event.u.CreateProcessInfo.hFile);
            break;
         case EXIT_PROCESS_DEBUG_EVENT:
            current = threads.get(dwCurrentThreadId);
            if (current) {
               current->refresh();
               exitCheckPoint = proceedCheckPoint();
            }
            threads.clear();
            current = NULL;
            started = false;
            break;
         case CREATE_THREAD_DEBUG_EVENT:
            current = new ThreadContext((*threads.start())->hProcess, event.u.CreateThread.hThread);
            current->refresh();

            threads.add(dwCurrentThreadId, current);
            break;
         case EXIT_THREAD_DEBUG_EVENT:
            threads.erase(event.dwThreadId);
            current = *threads.start();
            break;
         case LOAD_DLL_DEBUG_EVENT:
            ::CloseHandle(event.u.LoadDll.hFile);
            break;
         case UNLOAD_DLL_DEBUG_EVENT:
            break;
         case OUTPUT_DEBUG_STRING_EVENT:
            break;
         case RIP_EVENT:
            started = false;
            break;
         case EXCEPTION_DEBUG_EVENT:
            current = threads.get(dwCurrentThreadId);
            if (current) {
               current->refresh();
               processException(&event.u.Exception);
               current->refresh();
            }
            break;
      }
   }
}

bool x86DebugProcess :: proceed(int timeout)
{
   processEvent(timeout);

   return !trapped;
}

void x86DebugProcess :: resetException()
{
   
}

void x86DebugProcess :: run()
{
   continueProcess();
}

void x86DebugProcess :: activate()
{
   if (started) {
      EnumWindows(EnumThreadWndProc, dwCurrentThreadId);
   }
}

void x86DebugProcess :: stop()
{
   
}

void x86DebugProcess :: reset()
{
   trapped = false;

   //threads.clear();
   //current = NULL;

   //minAddress = 0xFFFFFFFF;
   //maxAddress = 0;

   //steps.clear();
   //breakpoints.clear();

   //init_breakpoint = 0;
   //stepMode = false;
   needToHandle = false;
   //exitCheckPoint = false;

   //_vmHook = 0;
}
