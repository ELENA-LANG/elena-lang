//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Debugger Adapter
//
//		This file contains DPA Session wrapper declaration
//
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef LDBG_SESSION_H
#define LDBG_SESSION_H

#include "dpa_session.h"
#include "ldbg_common.h"

namespace elena_lang
{
   class DPASessionWrapper
   {
      DPAEventManager _events;

      dpa::Session* _session;

   public:
      void prepare();
      void bind();
      void run();

      DPASessionWrapper();
      virtual ~DPASessionWrapper();
   };
}

#endif