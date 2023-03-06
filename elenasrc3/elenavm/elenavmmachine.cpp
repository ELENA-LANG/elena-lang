//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM declaration
//
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenavmmachine.h"
#include "langcommon.h"
#include "vmcommon.h"
#include "jitlinker.h"
#include "xmlprojectbase.h"
#include "bytecode.h"
#include "module.h"

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

      loadKeyCollection(config, root, EXTERNAL_CATEGORY,
         ProjectOption::Externals, ProjectOption::External, nullptr);
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
      : _mapper(this), _rootPath(configPath)
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

void ELENAVMMachine :: init(JITLinker& linker, SystemEnv* env)
{
   _presenter->print(ELENAVM_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELENAVM_REVISION_NUMBER);
   _presenter->print(ELENAVM_INITIALIZING);

   _compiler->populatePreloaded((uintptr_t)env, (uintptr_t)env->eh_table, (uintptr_t)env->gc_table);

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

bool ELENAVMMachine :: configurateVM(MemoryReader& reader, SystemEnv* env)
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
            eop = true;
            break;
         default:
            break;
      }
   }

   return true;
}

void ELENAVMMachine :: compileVMTape(MemoryReader& reader, MemoryDump& tapeSymbol, JITLinker& jitLinker, ModuleBase* dummyModule)
{
   MemoryWriter writer(&tapeSymbol);

   pos_t sizePlaceholder = writer.position();
   writer.writePos(0);

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
         case VM_CALLSYMBOL_CMD:
            ByteCodeUtil::write(writer, ByteCode::CallR,
               dummyModule->mapReference(strArg) | mskProcedureRef);
            break;
         default:
            break;
      }
   }

   ByteCodeUtil::write(writer, ByteCode::CloseN);
   ByteCodeUtil::write(writer, ByteCode::Quit);

   pos_t size = writer.position() - sizePlaceholder - sizeof(pos_t);

   writer.seek(sizePlaceholder);
   writer.writePos(size);
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

   JITLinker* jitLinker = nullptr;

   Module* dummyModule = new Module();
   MemoryDump tapeSymbol;

   if (configurateVM(reader, env)) {
      jitLinker = new JITLinker(&_mapper, &_libraryProvider, _configuration, dynamic_cast<ImageProviderBase*>(this),
         &_settings, nullptr);

      init(*jitLinker, env);
   }

   compileVMTape(reader, tapeSymbol, *jitLinker, dummyModule);

   SymbolList list;

   void* address = (void*)jitLinker->resolveTemporalByteCode(tapeSymbol, dummyModule);

   resumeVM(env, criricalHandler);

   freeobj(dummyModule);
   freeobj(jitLinker);

   return execute(env, address);
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

addr_t ELENAVMMachine :: resolveExternal(ustr_t referenceName)
{
   size_t index = referenceName.findLast('.');
   IdentifierString dll(referenceName, index);
   ustr_t functionName = referenceName + index + 1;

   if ((*dll).compare(RT_FORWARD)) {
      ustr_t resolvedName = _configuration->resolveExternal(*dll);
      if (!resolvedName.empty()) {
         dll.copy(resolvedName);
      }
   }

   return resolveExternal(*dll, functionName);
}

void ELENAVMMachine :: loadSubjectName(IdentifierString& actionName, ref_t subjectRef)
{
   
}

size_t ELENAVMMachine :: loadMessageName(mssg_t message, char* buffer, size_t length)
{
   //ref_t actionRef, flags;
   //pos_t argCount = 0;
   //decodeMessage(message, actionRef, argCount, flags);

   //IdentifierString actionName;
   //loadSubjectName(actionName, actionRef);

   //IdentifierString messageName;
   //ByteCodeUtil::formatMessageName(messageName, nullptr, *actionName, nullptr, 0, argCount, flags);

   //StrConvertor::copy(buffer, *messageName, messageName.length(), length);

   //return length;

   // !! temporal
   return 0;
}

size_t ELENAVMMachine :: loadAddressInfo(addr_t retPoint, char* lineInfo, size_t length)
{
   // !! temporal
   return 0;
}

addr_t ELENAVMMachine::loadClassReference(ustr_t name)
{
   // !! temporal
   return 0;
}

mssg_t ELENAVMMachine::loadMessage(ustr_t messageName)
{
   // !! temporal
   return 0;
}

addr_t ELENAVMMachine::loadSymbol(ustr_t name)
{
   // !! temporal
   return 0;
}

