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

namespace elena_lang
{
   class DPASessionWrapper
   {
   public:
      void prepare();

      DPASessionWrapper();
   };
}

#endif