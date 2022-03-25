//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the DebugController class and its helpers header
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef DEBUGCONTROLLER_H
#define DEBUGCONTROLLER_H

#include "idecommon.h"

namespace elena_lang
{
   // --- DebugController ---
   class DebugController : DebugControllerBase
   {
      bool              _started;
      bool              _running;
      PathString        _debuggee;
      PathString        _arguments;

      DebugProcessBase* _process;
      addr_t            _entryPoint;

      void debugThread() override;
      void processStep();

      bool startThread();

   public:
      bool isStarted() const
      {
         return _started;
      }

      bool start(path_t programPath, path_t arguments, bool debugMode);

      void run();

      DebugController(DebugProcessBase* process);
   };
   
}

#endif
