//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM declaration
//
//                                              (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENAVMMACHINE_H
#define ELENAVMMACHINE_H

#include "vmcommon.h"

namespace elena_lang
{
   // --- ELENARTMachine ---
   class ELENAVMMachine
   {
   public:
      ELENAVMMachine();

      virtual ~ELENAVMMachine()
      {
      }
   };
}

#endif