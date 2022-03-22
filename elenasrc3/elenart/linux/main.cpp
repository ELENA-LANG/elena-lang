//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Engine
//
//                                              (C)2021, by Aleksey Rakov
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

void InitializeSTA(SystemEnv* env, SymbolList* entryList)
{
   printf("InitializeSTA.2 %llx,%llx\n", (long long)env, (long long)entryList);
   fflush(stdout);

   if (machine != nullptr)
      init();

   machine->startSTA(env, entryList);
}
