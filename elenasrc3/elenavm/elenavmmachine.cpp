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

void ELENAVMMachine::ReferenceMapper :: addLazyReference(LazyReferenceInfo info)
{
   throw InternalError(errVMBroken);
}

List<LazyReferenceInfo>::Iterator ELENAVMMachine::ReferenceMapper :: lazyReferences()
{
   throw InternalError(errVMBroken);
}

void ELENAVMMachine::ReferenceMapper :: mapAction(ustr_t actionName, ref_t actionRef, ref_t signRef)
{
   throw InternalError(errVMBroken);
}

ustr_t ELENAVMMachine::ReferenceMapper :: retrieveAction(ref_t actionRef, ref_t& signRef)
{
   throw InternalError(errVMBroken);
}

void ELENAVMMachine::ReferenceMapper :: mapReference(ReferenceInfo referenceInfo, addr_t address, ref_t sectionMask)
{
   throw InternalError(errVMBroken);
}

ustr_t ELENAVMMachine::ReferenceMapper :: retrieveReference(addr_t address, ref_t sectionMask)
{
   throw InternalError(errVMBroken);
}

addr_t ELENAVMMachine::ReferenceMapper :: resolveReference(ReferenceInfo referenceInfo, ref_t sectionMask)
{
   throw InternalError(errVMBroken);
}

ref_t ELENAVMMachine::ReferenceMapper :: resolveAction(ustr_t actionName, ref_t signRef)
{
   throw InternalError(errVMBroken);
}

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
   throw InternalError(errVMBroken);
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

Section* ELENAVMMachine::getDataSection()
{
   throw InternalError(errVMBroken);
}

addr_t ELENAVMMachine::getDebugEntryPoint()
{
   throw InternalError(errVMBroken);
}

addr_t ELENAVMMachine::getEntryPoint()
{
   throw InternalError(errVMBroken);
}

Section* ELENAVMMachine::getImportSection()
{
   throw InternalError(errVMBroken);
}

Section* ELENAVMMachine::getMBDataSection()
{
   throw InternalError(errVMBroken);
}

Section* ELENAVMMachine::getMDataSection()
{
   throw InternalError(errVMBroken);
}

Section* ELENAVMMachine::getRDataSection()
{
   throw InternalError(errVMBroken);
}

Section* ELENAVMMachine::getStatSection()
{
   throw InternalError(errVMBroken);
}

Section* ELENAVMMachine::getTargetDebugSection()
{
   throw InternalError(errVMBroken);
}

Section* ELENAVMMachine::getTargetSection(ref_t targetMask)
{
   throw InternalError(errVMBroken);
}

Section* ELENAVMMachine::getTextSection()
{
   throw InternalError(errVMBroken);
}

AddressMap::Iterator ELENAVMMachine::externals()
{
   throw InternalError(errVMBroken);
}
