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
#include "linux/lnxconsts.h"

using namespace elena_lang;

static ELENARTMachine* machine = nullptr;
static SystemEnv* systemEnv = nullptr;

void init()
{
   machine = new ELENARTMachine(__routineProvider.RetrieveMDataPtr((void*)IMAGE_BASE, 0x1000000));
}

void InitializeSTLA(SystemEnv* env, SymbolList* entryList, void* criricalHandler)
{
   systemEnv = env;

#ifdef DEBUG_OUTPUT
   printf("InitializeSTA.6 %llx,%llx,%llx\n", (long long)env, (long long)entryList, (long long)criricalHandler);
   fflush(stdout);
#endif

   if (machine == nullptr)
      init();

   __routineProvider.InitExceptionHandling(env, criricalHandler);

   machine->startSTA(env, entryList);
}

void* CollectGCLA(void* roots, size_t size)
{
   printf("CollectGCLA %llx %llx\n", (long long)roots, size);

   return __routineProvider.GCRoutine(systemEnv->gc_table, (GCRoot*)roots, size);
}

size_t LoadMessageNameLA(size_t message, char* buffer, size_t length)
{
   return machine->loadMessageName((mssg_t)message, buffer, length);
}

void ExitLA(int retVal)
{
   if (retVal) {
      printf("Aborted:%x\n", retVal);
      fflush(stdout);
   }
   __routineProvider.Exit(retVal);
}
