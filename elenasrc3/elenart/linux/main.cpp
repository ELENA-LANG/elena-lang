//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// -------------------------------------------------------------------
#include <unistd.h>

#include "elenart.h"
#include "elenartmachine.h"

using namespace elena_lang;

static ELENARTMachine* machine = nullptr;

void init()
{
   machine = new ELENARTMachine();
}

void InitializeSTLA(SystemEnv* env, SymbolList* entryList, void* criricalHandler)
{
   printf("InitializeSTA.4 %llx,%llx,%llx\n", (long long)env, (long long)entryList, (long long)criricalHandler);
   fflush(stdout);

   if (machine != nullptr)
      init();

   __routineProvider.InitExceptionHandling(env, criricalHandler);

   machine->startSTA(env, entryList);
}

void ExitLA(int retVal)
{
   if (retVal) {
      printf("Aborted:%x\n", retVal);
      fflush(stdout);
   }
   __routineProvider.Exit(retVal);
}
