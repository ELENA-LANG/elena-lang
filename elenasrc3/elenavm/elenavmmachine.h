//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM declaration
//
//                                              (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef ELENAVMMACHINE_H
#define ELENAVMMACHINE_H

#include "vmcommon.h"
#include "libman.h"
#include "elenamachine.h"

namespace elena_lang
{
   constexpr auto ELENAVM_GREETING        = "ELENA VM %d.%d.%d (C)2022 by Aleksex Rakov";
   constexpr auto ELENAVM_INITIALIZING    = "Initializing...";

   // --- ELENARTMachine ---
   class ELENAVMMachine : public ELENAMachine
   {
      bool            _initialized;
      LibraryProvider _libraryProvider;
      PresenterBase*  _presenter;

      int interprete(SystemEnv* env, void* tape, pos_t size, void* criricalHandler);

      void onNewCode();

      void stopVM();
      void* evaluateVMTape(MemoryReader& reader);

      void resumeVM(SystemEnv* env, void* criricalHandler);

      void init();

   public:
      void startSTA(SystemEnv* env, void* tape, void* criricalHandler);

      void Exit(int exitCode);

      ELENAVMMachine(PresenterBase* presenter);
      virtual ~ELENAVMMachine()
      {
      }
   };
}

#endif