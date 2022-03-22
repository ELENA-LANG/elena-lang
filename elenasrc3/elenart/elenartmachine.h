//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Machine declaration
//
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENARTMACHINE_H
#define ELENARTMACHINE_H

#include "rtcommon.h"

namespace elena_lang
{
   // --- ELENARTMachine ---
   class ELENARTMachine
   {
      void Exit(int exitCode);

   public:
      void startSTA(SystemEnv* env, SymbolList* entryList);

      ELENARTMachine();

      virtual ~ELENARTMachine()
      {

      }
   };
}

#endif