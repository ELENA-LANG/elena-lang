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
      void* mdata;

      void Exit(int exitCode);

   public:
      void startSTA(SystemEnv* env, SymbolList* entryList);

      void loadSubjectName(IdentifierString& actionName, ref_t subjectRef);
      size_t loadMessageName(mssg_t messageRef, char* buffer, size_t length);

      ELENARTMachine(void* mdata);

      virtual ~ELENARTMachine()
      {
      }
   };
}

#endif