//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM declaration
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenavmmachine.h"
#include "langcommon.h"
#include "vmcommon.h"

using namespace elena_lang;

// --- ELENARTMachine ---

ELENAVMMachine :: ELENAVMMachine(PresenterBase* presenter)
{
   _initialized = false;
   _presenter = presenter;
}

void ELENAVMMachine :: init()
{
   _presenter->print(ELENAVM_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELENAVM_REVISION_NUMBER);
   _presenter->print(ELENAVM_INITIALIZING);
}

void ELENAVMMachine :: stopVM()
{
   
}

void* ELENAVMMachine :: evaluateVMTape(MemoryReader& reader)
{
   void* retVal = nullptr;

   pos_t  command = 0;
   ustr_t strArg = nullptr;
   bool eop = false;
   while (!eop) {
      command = reader.getDWord();
      if (test(command, VM_STR_COMMAND_MASK))
         strArg = reader.getString(nullptr);

      switch (command) {
         case VM_SETNAMESPACE:
            _libraryProvider.setNamespace(strArg);
            break;
         case VM_SETPACKAGEPATH:
         {
            PathString pathArg(strArg);

            _libraryProvider.setRootPath(*pathArg);
            break;
         }
         case VM_INIT:
            init();
            break;
         case VM_ENDOFTAPE:
            eop = true;
            break;
         default:
            break;
      }
   }

   return retVal;
}

void ELENAVMMachine :: onNewCode()
{
   
}

void ELENAVMMachine :: resumeVM(SystemEnv* env, void* criricalHandler)
{
   if (!_initialized)
      throw InternalError(errVMNotInitialized);

   onNewCode();

   __routineProvider.InitExceptionHandling(env, criricalHandler);
}

int ELENAVMMachine :: interprete(SystemEnv* env, void* tape, pos_t size, void* criricalHandler)
{
   ByteArray      tapeArray(tape, size);
   MemoryReader   reader(&tapeArray);

   stopVM();

   void* entryList = evaluateVMTape(reader);

   resumeVM(env, criricalHandler);

   if (!entryList)
      throw InternalError(errVMNotExecuted);

   return execute(env, (SymbolList*)entryList);
}

void ELENAVMMachine :: startSTA(SystemEnv* env, void* tape, void* criricalHandler)
{
   // setting up system
   __routineProvider.InitSTA(env);

   int retVal = -1;
   if (tape != nullptr) {
      retVal = interprete(env, tape, INVALID_POS, criricalHandler);
   }

   Exit(retVal);
}

void ELENAVMMachine :: Exit(int exitCode)
{
   __routineProvider.Exit(exitCode);
}
