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
#include "jitlinker.h"

using namespace elena_lang;

// --- ELENARTMachine::ReferenceMapper ---

// --- ELENARTMachine::Configuration ---

void ELENAVMMachine::Configuration :: addForward(ustr_t forward, ustr_t referenceName)
{
   throw InternalError(errVMBroken);
}

ustr_t ELENAVMMachine::Configuration :: resolveExternal(ustr_t forward)
{
   throw InternalError(errVMBroken);
}

ustr_t ELENAVMMachine::Configuration :: resolveForward(ustr_t forward)
{
   // !! temporal
   return nullptr;
}

ustr_t ELENAVMMachine::Configuration :: resolveWinApi(ustr_t forward)
{
   throw InternalError(errVMBroken);
}

// --- ELENARTMachine ---

ELENAVMMachine :: ELENAVMMachine(PresenterBase* presenter, PlatformType platform,
   JITCompilerBase* (*jitCompilerFactory)(LibraryLoaderBase*, PlatformType))
      : _configuration(platform)
{
   _initialized = false;
   _presenter = presenter;

   _compiler = jitCompilerFactory(&_libraryProvider, _configuration.platform);
}

void ELENAVMMachine :: init(JITLinker& linker)
{
   _presenter->print(ELENAVM_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELENAVM_REVISION_NUMBER);
   _presenter->print(ELENAVM_INITIALIZING);

   linker.prepare(_compiler);

   _initialized = true;
}

void ELENAVMMachine :: stopVM()
{
   
}

void* ELENAVMMachine :: evaluateVMTape(MemoryReader& reader)
{
   void* retVal = nullptr;

   JITLinker jitLinker(&_mapper, &_libraryProvider, &_configuration, dynamic_cast<ImageProviderBase*>(this), 
      &_configuration.settings, nullptr);

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
            init(jitLinker);
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

AddressMap::Iterator ELENAVMMachine::externals()
{
   throw InternalError(errVMBroken);
}
