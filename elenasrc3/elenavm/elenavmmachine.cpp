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

   // configurate the loader
   _configuration->initLoader(_libraryProvider);

   _compiler = jitCompilerFactory(&_libraryProvider, platform);
}

void ELENAVMMachine :: init(JITLinker& linker, SystemEnv* exeEnv)
{
   _presenter->print(ELENAVM_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELENAVM_REVISION_NUMBER);
   _presenter->print(ELENAVM_INITIALIZING);

   _compiler->populatePreloaded((uintptr_t)exeEnv->th_table);

   linker.prepare(_compiler);

   _env = (SystemEnv*)_compiler->getSystemEnv();

   // setting up system
   __routineProvider.InitSTA(_env);

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
   _settings.jitSettings.ygSize = env->gc_yg_size;
   _settings.jitSettings.mgSize = env->gc_mg_size;

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
         case VM_PRELOADED_CMD:
            _preloadedSection.copy(strArg);
            break;
         default:
            break;
      }
   }

   return true;
}

void ELENAVMMachine :: fillPreloadedSymbols(MemoryWriter& writer, ModuleBase* dummyModule)
{
   ModuleInfoList symbolList({});
   _mapper.forEachLazyReference<ModuleInfoList*>(&symbolList, [](ModuleInfoList* symbolList, LazyReferenceInfo info)
      {
         if (info.mask == mskAutoSymbolRef) {
            symbolList->add({ info.module, info.reference });
         }
      });

   for (auto it = symbolList.start(); !it.eof(); ++it) {
      auto info = *it;
      ustr_t symbolName = info.module->resolveReference(info.reference);
      if (isWeakReference(symbolName)) {
         IdentifierString fullName(info.module->name(), symbolName);

         ByteCodeUtil::write(writer, ByteCode::CallR, dummyModule->mapReference(*fullName) | mskSymbolRef);
      }
      else ByteCodeUtil::write(writer, ByteCode::CallR, dummyModule->mapReference(symbolName) | mskSymbolRef);
   }

}

inline ref_t getCmdMask(int command)
{
   switch (command) {
      case VM_CALLSYMBOL_CMD:
         return mskSymbolRef;
      case VM_CALLCLASS_CMD:
         return mskVMTRef;
      default:
         return 0;
   }
}

void ELENAVMMachine :: compileVMTape(MemoryReader& reader, MemoryDump& tapeSymbol, JITLinker& jitLinker, ModuleBase* dummyModule)
{
   CachedList<ref_t, 5> symbols;

   pos_t  command = 0;
   ustr_t strArg = nullptr;
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
         case VM_CALLCLASS_CMD:
            jitLinker.resolve(strArg, getCmdMask(command), false);
            symbols.add(dummyModule->mapReference(strArg) | getCmdMask(command));
            break;
         default:
            break;
      }
   }

   MemoryWriter writer(&tapeSymbol);

   pos_t sizePlaceholder = writer.position();
   writer.writePos(0);

   ByteCodeUtil::write(writer, ByteCode::OpenIN, 2, 0);

   fillPreloadedSymbols(writer, dummyModule);
   for(size_t i = 0; i < symbols.count(); i++) {
      ref_t mask = symbols[i] & mskAnyRef;

      switch (mask){
         case mskSymbolRef:
            ByteCodeUtil::write(writer, ByteCode::CallR, symbols[i]);
            break;
         case mskVMTRef:
            ByteCodeUtil::write(writer, ByteCode::SetR, symbols[i]);
            break;
         default:
            assert(false);
            break;
      }
   }

   ByteCodeUtil::write(writer, ByteCode::CloseN);
   ByteCodeUtil::write(writer, ByteCode::Quit);

   pos_t size = writer.position() - sizePlaceholder - sizeof(pos_t);

   writer.seek(sizePlaceholder);
   writer.writePos(size);
}

void ELENAVMMachine :: onNewCode(JITLinker& jitLinker)
{
   ustr_t superClass = _configuration->resolveForward(SUPER_FORWARD);

   jitLinker.complete(_compiler, superClass);

   _mapper.clearLazyReferences();
}

void ELENAVMMachine :: resumeVM(JITLinker& jitLinker, SystemEnv* env, void* criricalHandler)
{
   if (!_initialized)
      throw InternalError(errVMNotInitialized);

   onNewCode(jitLinker);

   if (criricalHandler)
      __routineProvider.InitSTAExceptionHandling(env, criricalHandler);
}

addr_t ELENAVMMachine :: interprete(SystemEnv* env, void* tape, pos_t size, const char* criricalHandlerReference)
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

      if (!_initialized) {
         init(*jitLinker, env);
      }
      else jitLinker->setCompiler(_compiler);
   }

   addr_t criricalHandler = _initialized ? 0 : jitLinker->resolve(criricalHandlerReference, mskProcedureRef, false);

   compileVMTape(reader, tapeSymbol, *jitLinker, dummyModule);

   SymbolList list;

   void* address = (void*)jitLinker->resolveTemporalByteCode(tapeSymbol, dummyModule);

   resumeVM(*jitLinker, env, (void*)criricalHandler);

   freeobj(dummyModule);
   freeobj(jitLinker);

   return execute(env, address);
}

void ELENAVMMachine :: startSTA(SystemEnv* env, void* tape, const char* criricalHandlerReference)
{
   int retVal = -1;
   if (tape != nullptr) {
      interprete(env, tape, INVALID_POS, criricalHandlerReference);

      retVal = 0;
   }

   Exit(retVal);
}

void ELENAVMMachine :: Exit(int exitCode)
{
   __routineProvider.Exit(exitCode);
}

AddressMap::Iterator ELENAVMMachine :: externals()
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

inline void addVMTapeEntry(MemoryWriter& rdataWriter, pos_t command, ustr_t arg)
{
   rdataWriter.writeDWord(command);
   rdataWriter.writeString(arg);
}

inline void addVMTapeEntry(MemoryWriter& rdataWriter, pos_t command)
{
   rdataWriter.writeDWord(command);
}

addr_t ELENAVMMachine :: loadReference(ustr_t name, int command)
{
   MemoryDump tape;
   MemoryWriter tapeWriter(&tape);

   addVMTapeEntry(tapeWriter, VM_INIT_CMD);
   addVMTapeEntry(tapeWriter, command, name);
   addVMTapeEntry(tapeWriter, VM_ENDOFTAPE_CMD);

   interprete(_env, tape.get(0), tape.length(), nullptr);

   return _mapper.resolveReference({ nullptr, name }, getCmdMask(command));
}

addr_t ELENAVMMachine :: loadClassReference(ustr_t name)
{
   return loadReference(name, VM_CALLCLASS_CMD);
}

ref_t ELENAVMMachine :: loadSubject(ustr_t actionName)
{
   stopVM();

   JITLinker* jitLinker = new JITLinker(&_mapper, &_libraryProvider, _configuration, dynamic_cast<ImageProviderBase*>(this),
         &_settings, nullptr);

   jitLinker->setCompiler(_compiler);

   ref_t actionRef = jitLinker->resolveAction(actionName);

   resumeVM(*jitLinker, _env, nullptr);

   return actionRef;
}

mssg_t ELENAVMMachine :: loadMessage(ustr_t messageName)
{
   pos_t argCount = 0;
   ref_t flags = 0;

   IdentifierString actionName;
   ByteCodeUtil::parseMessageName(messageName, actionName, flags, argCount);

   ref_t actionRef = loadSubject(*actionName);
   if (!actionRef)
      return 0;

   return encodeMessage(actionRef, argCount, flags);
}

addr_t ELENAVMMachine :: loadSymbol(ustr_t name)
{
   return loadReference(name, VM_CALLSYMBOL_CMD);
}

//void ELENAVMMachine :: generateAutoSymbol(ModuleInfoList& list, ModuleBase* module, MemoryDump& tapeSymbol)
//{
//   MemoryWriter writer(&tapeSymbol);
//
//   pos_t sizePlaceholder = writer.position();
//   writer.writePos(0);
//
//   pos_t  command = 0;
//   ustr_t strArg = nullptr;
//
//   ByteCodeUtil::write(writer, ByteCode::OpenIN, 2, 0);
//
//   // generate the preloaded list
//   for (auto it = list.start(); !it.eof(); ++it) {
//      auto info = *it;
//      ustr_t symbolName = info.module->resolveReference(info.reference);
//      IdentifierString fullName(info.module->name(), symbolName);
//
//      ByteCodeUtil::write(writer, ByteCode::CallR, module->mapReference(*fullName) | mskSymbolRef);
//   }
//
//   ByteCodeUtil::write(writer, ByteCode::CloseN);
//   ByteCodeUtil::write(writer, ByteCode::Quit);
//
//   pos_t size = writer.position() - sizePlaceholder - sizeof(pos_t);
//
//   writer.seek(sizePlaceholder);
//   writer.writePos(size);
//}
