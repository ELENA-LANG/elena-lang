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
#include "projectbase.h"

using namespace elena_lang;

// --- ELENARTMachine::ReferenceMapper ---

// --- ELENAVMConfiguration ---

class ELENAVMConfiguration : public XmlProjectBase
{
protected:
   void loadConfig(ConfigFile& config, path_t configPath, ConfigFile::Node root)
   {
      loadPathCollection(config, root, PRIMITIVE_CATEGORY,
         ProjectOption::Primitives, configPath);
   }

   bool loadConfig(path_t path)
   {
      ConfigFile config;
      if (config.load(path, FileEncoding::UTF8)) {
         PathString configPath;
         configPath.copySubPath(path, false);

         ConfigFile::Node root = config.selectRootNode();
         // select platform configuration
         ConfigFile::Node platformRoot = getPlatformRoot(config, _platform);

         loadConfig(config, *configPath, root);
         loadConfig(config, *configPath, platformRoot);

         return true;
      }

      return false;
   }

public:
   void addForward(ustr_t forward, ustr_t referenceName)
   {
      throw InternalError(errVMBroken);
   }

   ustr_t resolveExternal(ustr_t forward)
   {
      throw InternalError(errVMBroken);
   }

   ustr_t resolveForward(ustr_t forward)
   {
      // !! temporal
      return nullptr;
   }

   ustr_t resolveWinApi(ustr_t forward)
   {
      throw InternalError(errVMBroken);
   }

   void forEachForward(void* arg, void (*feedback)(void* arg, ustr_t key, ustr_t value))
   {

   }

   ELENAVMConfiguration(PlatformType platform, path_t path)
      : XmlProjectBase(platform)
   {
      loadConfig(path);
   }
};


// --- ELENARTMachine ---

ELENAVMMachine :: ELENAVMMachine(path_t configPath, PresenterBase* presenter, PlatformType platform,
   JITCompilerBase* (*jitCompilerFactory)(LibraryLoaderBase*, PlatformType))
{
   _initialized = false;
   _presenter = presenter;

   _configuration = new ELENAVMConfiguration(platform, configPath);

   _settings.autoLoadMode = _configuration->BoolSetting(ProjectOption::ClassSymbolAutoLoad);

   imageInfo.codeAlignment,
      imageInfo.coreSettings,
      true,
      imageInfo.autoClassSymbol



   

   _settings.codeAlignment = _codeAlignment;
   imageInfo.autoClassSymbol = project.BoolSetting(ProjectOption::ClassSymbolAutoLoad);
   imageInfo.coreSettings.mgSize = project.IntSetting(ProjectOption::GCMGSize, _defaultCoreSettings.mgSize);
   imageInfo.coreSettings.ygSize = project.IntSetting(ProjectOption::GCYGSize, _defaultCoreSettings.ygSize);
   imageInfo.ns = project.StringSetting(ProjectOption::Namespace);


   _compiler = jitCompilerFactory(&_libraryProvider, platform);
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

   JITLinker jitLinker(&_mapper, &_libraryProvider, _configuration, dynamic_cast<ImageProviderBase*>(this), 
      &_settings, nullptr);

   pos_t  command = 0;
   ustr_t strArg = nullptr;
   bool eop = false;
   while (!eop) {
      command = reader.getDWord();
      if (test(command, VM_STR_COMMAND_MASK))
         strArg = reader.getString(nullptr);

      switch (command) {
         case VM_SETNAMESPACE_CMD:
            _libraryProvider.setNamespace(strArg);
            break;
         case VM_SETPACKAGEPATH_CMD:
         {
            PathString pathArg(strArg);

            _libraryProvider.setRootPath(*pathArg);
            break;
         }
         case VM_INIT_CMD:
            init(jitLinker);
            break;
         case VM_ENDOFTAPE_CMD:
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
