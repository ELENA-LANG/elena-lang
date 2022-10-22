//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Machine declaration
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenartmachine.h"

using namespace elena_lang;

// --- ELENARTMachine ---

void ELENARTMachine :: Exit(int exitCode)
{
   __routineProvider.Exit(exitCode);
}

void ELENARTMachine :: startSTA(SystemEnv* env, SymbolList* entryList)
{
   // setting up system
   __routineProvider.InitSTA(env);

   Entry entry;
   entry.address = env->bc_invoker;

   // executing the program
   int retVal = 0;
   for (size_t i = 0; i < entryList->length; i += sizeof(intptr_t)) {
      try
      {
         retVal = entry.evaluate(entryList->entries[i].address, 0);

         printf("exiting\n");
         fflush(stdout);
      }
      catch (InternalError&)
      {
         //_instance->printInfo("EAbortException");

         retVal = -1;
      }
   }

   // winding down system
   Exit(retVal);
}

ELENARTMachine :: ELENARTMachine()
{

}