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
#include "xmlprojectbase.h"
#include "bytecode.h"

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
      loadPathCollection(config, root, REFERENCE_CATEGORY,
         ProjectOption::References, configPath);

      loadPathSetting(config, root, LIB_PATH, ProjectOption::LibPath, configPath);
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
   ustr_t resolveExternal(ustr_t forward)
   {
      throw InternalError(errVMBroken);
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
   int codeAlignment, JITSettings gcSettings,
   JITCompilerBase* (*jitCompilerFactory)(LibraryLoaderBase*, PlatformType))
{
   _initialized = false;
   _presenter = presenter;

   _configuration = new ELENAVMConfiguration(platform, configPath);

   _settings.autoLoadMode = _configuration->BoolSetting(ProjectOption::ClassSymbolAutoLoad);
   _settings.virtualMode = false;
   _settings.alignment = codeAlignment;
   _settings.jitSettings.mgSize = _configuration->IntSetting(ProjectOption::GCMGSize, gcSettings.mgSize);
   _settings.jitSettings.ygSize = _configuration->IntSetting(ProjectOption::GCYGSize, gcSettings.ygSize);

   // configurate the loader
   _configuration->initLoader(_libraryProvider);

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

void ELENAVMMachine :: addForward(ustr_t forwardLine)
{
   pos_t index = forwardLine.find('=');
   if (index != NOTFOUND_POS) {
      ustr_t fullReference = forwardLine + index + 1;

      IdentifierString fwd(forwardLine, index);

      _configuration->addForward(*fwd, fullReference);
   }
}

void ELENAVMMachine :: addPackage(ustr_t packageLine)
{
   pos_t index = packageLine.find('=');
   if (index != NOTFOUND_POS) {
      PathString path(packageLine + index + 1);
      IdentifierString ns(packageLine, index);

      _libraryProvider.addPackage(*ns, *path);
   }
}

void ELENAVMMachine :: configurateVM(MemoryReader& reader, JITLinker& jitLinker)
{
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
         case VM_FORWARD_CMD:
            addForward(strArg);
            break;
         case VM_PACKAGE_CMD:
            addPackage(strArg);
            break;
         case VM_INIT_CMD:
            init(jitLinker);
            eop = true;
            break;
         default:
            break;
      }
   }
}

void ELENAVMMachine :: compileVMTape(MemoryReader& reader, MemoryDump& tapeSymbol, JITLinker& jitLinker)
{
   MemoryWriter writer(&tapeSymbol);

   pos_t  command = 0;
   ustr_t strArg = nullptr;

   ByteCodeUtil::write(writer, ByteCode::OpenIN, 2, 0);

   bool eop = false;
   while (!eop) {
      command = reader.getDWord();
      if (test(command, VM_STR_COMMAND_MASK))
         strArg = reader.getString(nullptr);

      switch (command) {
         case VM_ENDOFTAPE_CMD:
            eop = true;
            break;
         case VM_LOADSYMBOLARRAY_CMD:
         {
            addr_t address = jitLinker.resolve(strArg, mskTypeListRef, true);
            if (address == INVALID_ADDR)
               throw JITUnresolvedException(ReferenceInfo { strArg });

            loadSymbolArrayList(writer, (void*)address);
            break;
         }
         default:
            break;
      }
   }

   ByteCodeUtil::write(writer, ByteCode::CloseN);
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

   JITLinker jitLinker(&_mapper, &_libraryProvider, _configuration, dynamic_cast<ImageProviderBase*>(this),
      &_settings, nullptr);

   MemoryDump tapeSymbol;
   JITLinker::JITLinkerReferenceHelper helper(&jitLinker, nullptr, nullptr);

   configurateVM(reader, jitLinker);
   compileVMTape(reader, tapeSymbol, jitLinker);

   SymbolList list;

   list.length = sizeof(intptr_t);
   list.entries[0].address = (void*)jitLinker.resolveTemporalByteCode(helper, tapeSymbol);

   resumeVM(env, criricalHandler);

   return execute(env, &list);
}

void ELENAVMMachine :: loadSymbolArrayList(MemoryWriter& writer, void* address)
{
   SymbolList* list = (SymbolList*)address;
   for (size_t i = 0; i < list->length; i++) {
      
   }
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
