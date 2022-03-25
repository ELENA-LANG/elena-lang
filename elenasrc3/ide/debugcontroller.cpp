//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains implematioon of the DebugController class and
//    its helpers
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "debugcontroller.h"

using namespace elena_lang;

// --- DebugController ---

DebugController :: DebugController(DebugProcessBase* process)
{
   _started = false;
   _process = process;
   _entryPoint = 0;
}

void DebugController :: debugThread()
{
   if (!_process->startProgram(_debuggee.str(), _arguments.str())) {
      //HOTFIX : to inform the listening thread
      _process->resetEvent(DEBUG_ACTIVE);

      return;
   }

   _running = true;
   while (_process->isStarted()) {
      int event = _process->waitForAnyEvent();

      switch (event) {
         case DEBUG_ACTIVE:
            if (_process->Exception()) {
               processStep();
               _process->resetException();
               _process->run();
            }
            else if (!_process->proceed(100)) {
               _process->resetEvent(DEBUG_ACTIVE);
               _running = false;
               processStep();
            }
            else _process->run();

            break;
         case DEBUG_RESUME:
            _running = true;
            _process->run();
            _process->resetEvent(DEBUG_RESUME);
            _process->setEvent(DEBUG_ACTIVE);
            break;
         case DEBUG_SUSPEND:
            _running = false;
            _process->resetEvent(DEBUG_SUSPEND);
            _process->resetEvent(DEBUG_ACTIVE);
            break;
         case DEBUG_CLOSE:
            _process->stop();
            _process->setEvent(DEBUG_ACTIVE);
            _process->resetEvent(DEBUG_CLOSE);
            break;
         default:
            break;
      }
   }
   _running = false;
   //_currentModule = nullptr;
   _process->clearEvents();

   //_listener->onStop(_process->proceedCheckPoint());
}

void DebugController :: processStep()
{
   
}

void DebugController :: run()
{
   if (_running || !_process->isStarted())
      return;

   //if (_debugInfoPtr == 0 && _entryPoint != 0) {
   //   _process->setBreakpoint(_entryPoint, false);
   //   //_postponed.clear();
   //}
   _started = true;

   _process->setEvent(DEBUG_RESUME);

   _process->activate();
}

bool DebugController :: startThread()
{
   _process->initEvents();
   _process->setEvent(DEBUG_SUSPEND);

   _process->startThread(this);

   while (_process->waitForEvent(DEBUG_ACTIVE, 0));

   //_listener->onStart();

   return _process->isStarted();
}

bool DebugController :: start(path_t programPath, path_t arguments, bool debugMode)
{
   //_currentModule = NULL;
   _started = false;

   _process->reset();

   _debuggee.copy(programPath);
   _arguments.copy(arguments);

   if (debugMode) {
      _entryPoint = _process->findEntryPoint(programPath);
      if (_entryPoint == INVALID_ADDR)
         return false;

      _debugger.initHook();
   }
   else {
      _entryPoint = 0;

      clearBreakpoints();
   }

   return startThread();
}
