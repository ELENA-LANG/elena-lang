//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Engine
//
//		This file contains the DebugController class and its helpers header
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef DEBUGCONTROLLER_H
#define DEBUGCONTROLLER_H

namespace elena_lang
{
   // --- DebugController ---
   class DebugController
   {
      bool _started;

   public:
      bool isStarted() const
      {
         return _started;
      }

      void run();

      DebugController();
   };
   
}

#endif
