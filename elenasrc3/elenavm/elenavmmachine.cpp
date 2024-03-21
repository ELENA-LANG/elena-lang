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
#include "rtmanager.h"

using namespace elena_lang;

// --- ELENAVMConfiguration ---

void ELENAVMConfiguration :: loadConfig(ConfigFile& config, path_t configPath, ConfigFile::Node root)
{
   ConfigFile::Node baseConfig = config.selectNode(root, PROJECT_TEMPLATE);
   if (!baseConfig.isNotFound()) {
      DynamicString<char> key;
      baseConfig.readContent(key);

      loadConfigByName(configPath, key.str());
   }

   loadPathCollection(config, root, TEMPLATE_CATEGORY,
      ProjectOption::Templates, configPath);

   loadPathCollection(config, root, PRIMITIVE_CATEGORY,
      ProjectOption::Primitives, configPath);
   loadPathCollection(config, root, REFERENCE_CATEGORY,
      ProjectOption::References, configPath);

   loadPathSetting(config, root, LIB_PATH, ProjectOption::LibPath, configPath);

   loadKeyCollection(config, root, EXTERNAL_CATEGORY,
      ProjectOption::Externals, ProjectOption::External, nullptr);

   loadForwards(config, root, FORWARD_CATEGORY);
}

bool ELENAVMConfiguration :: loadConfig(path_t path)
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

ustr_t ELENAVMConfiguration :: resolveWinApi(ustr_t forward)
{
   throw InternalError(errVMBroken);
}

bool ELENAVMConfiguration :: loadConfigByName(path_t configPath, ustr_t name)
{
   path_t relativePath = PathSetting(ProjectOption::Templates, name);
   if (!relativePath.empty()) {
      PathString baseConfigPath(configPath, relativePath);

      return loadConfig(*baseConfigPath);
   }
   return false;
}

void ELENAVMConfiguration :: forEachForward(void* arg, void (*feedback)(void* arg, ustr_t key, ustr_t value))
{
}

// --- ELENAVMMachine ---

ELENAVMMachine :: ELENAVMMachine(path_t configPath, PresenterBase* presenter, PlatformType platform, 
   int codeAlignment, JITSettings gcSettings,
   JITCompilerBase* (*jitCompilerFactory)(LibraryLoaderBase*, PlatformType))
      : _mapper(this), _rootPath(configPath), _preloadedList({})
{
   _standAloneMode = true;
   _initialized = false;
   _presenter = presenter;
   _env = nullptr;
   _startUpCode = true;

   _configuration = new ELENAVMConfiguration(platform, configPath);

   _settings.autoLoadMode = _configuration->BoolSetting(ProjectOption::ClassSymbolAutoLoad);
   _settings.virtualMode = false;
   _settings.alignment = codeAlignment;

   _libraryProvider.addListener(this);

   _compiler = jitCompilerFactory(&_libraryProvider, platform);
}

void ELENAVMMachine :: init(JITLinker& linker, SystemEnv* exeEnv)
{
   _presenter->printLine(ELENAVM_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELENAVM_REVISION_NUMBER);
   _presenter->printLine(ELENAVM_INITIALIZING);

   _configuration->initLoader(_libraryProvider);

   _compiler->populatePreloaded(
      (uintptr_t)exeEnv->th_table,
      (uintptr_t)exeEnv->th_single_content);

   linker.setCompiler(_compiler);

   linker.prepare();

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
   size_t index = forwardLine.find('=');
   if (index != NOTFOUND_POS) {
      ustr_t fullReference = forwardLine + index + 1;

      IdentifierString fwd(forwardLine, index);

      _configuration->addForward(*fwd, fullReference);
   }
}

void ELENAVMMachine :: addPackage(ustr_t packageLine)
{
   size_t index = packageLine.find('=');
   if (index != NOTFOUND_POS) {
      PathString path(packageLine + index + 1);
      IdentifierString ns(packageLine, index);

      _libraryProvider.addPackage(*ns, *path);
   }
}

bool ELENAVMMachine :: loadConfig(ustr_t configName)
{
   return _configuration->loadConfigByName(_rootPath, configName);
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
         case VM_CONFIG_CMD:
            if (!loadConfig(strArg))
               return false;
            break;
         case VM_TERMINAL_CMD:
            _standAloneMode = false;
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

void ELENAVMMachine :: fillPreloadedSymbols(JITLinker& jitLinker, MemoryWriter& writer, ModuleBase* dummyModule)
{
   ModuleInfoList symbolList({});
   for (auto it = _preloadedList.start(); !it.eof(); ++it) {
      jitLinker.copyMetaList(*it, symbolList);
   }
   _preloadedList.clear();

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
      case VM_TRYCALLSYMBOL_CMD:
          return mskSymbolRef;
      case VM_CALLCLASS_CMD:
         return mskVMTRef;
      case VM_STRING_CMD:
         return mskLiteralRef;
      default:
         return 0;
   }
}

bool ELENAVMMachine :: compileVMTape(MemoryReader& reader, MemoryDump& tapeSymbol, JITLinker& jitLinker, ModuleBase* dummyModule)
{
   CachedList<Pair<ref_t, pos_t>, 5> symbols;

   pos_t  command = 0;
   ustr_t strArg = nullptr;
   ref_t  nArg = 0;
   bool   eop = false;
   while (!eop) {
      command = reader.getDWord();
      if (test(command, VM_STR_COMMAND_MASK))
         strArg = reader.getString(nullptr);
      else if (test(command, VM_INT_COMMAND_MASK))
         nArg = reader.getDWord();

      switch (command) {
         case VM_ENDOFTAPE_CMD:
            eop = true;
            break;
         case VM_CALLSYMBOL_CMD:
         case VM_CALLCLASS_CMD:
            jitLinker.resolve(strArg, getCmdMask(command), false);
            symbols.add({ dummyModule->mapReference(strArg) | getCmdMask(command), 0 });
            break;
         case VM_TRYCALLSYMBOL_CMD:
            if (jitLinker.resolve(strArg, getCmdMask(command), true) != INVALID_ADDR) {
               symbols.add({ dummyModule->mapReference(strArg) | getCmdMask(command), 0 });
            }
            else return false;
            break;
         case VM_STRING_CMD:
            jitLinker.resolve(strArg, getCmdMask(command), false);
            symbols.add({ dummyModule->mapConstant(strArg) | getCmdMask(command), 0 });
            break;
         case VM_ALLOC_CMD:
         case VM_FREE_CMD:
         case VM_SET_ARG_CMD:
            symbols.add({ nArg, command });
            break;
         case VM_NEW_CMD:
            symbols.add({ dummyModule->mapReference(strArg) | mskVMTRef, nArg });
            break;
         case VM_SEND_MESSAGE_CMD:
            symbols.add({ ByteCodeUtil::resolveMessageName(strArg, dummyModule, false), command });
            break;
         default:
            break;
      }
   }

   if (symbols.count() == 0 && _preloadedList.count() == 0)
      return false;

   MemoryWriter writer(&tapeSymbol);

   pos_t sizePlaceholder = writer.position();
   writer.writePos(0);

   ByteCodeUtil::write(writer, ByteCode::OpenIN, 2, 0);

   fillPreloadedSymbols(jitLinker, writer, dummyModule);

   for(size_t i = 0; i < symbols.count(); i++) {
      auto p = symbols[i];
      ref_t mask = p.value1 & mskAnyRef;
      if (!p.value2) {
         switch (mask) {
            case mskSymbolRef:
               ByteCodeUtil::write(writer, ByteCode::CallR, p.value1);
               break;
            case mskVMTRef:
            case mskLiteralRef:
               ByteCodeUtil::write(writer, ByteCode::SetR, p.value1);
               break;
            default:
               assert(false);
               break;
         }
      }
      else if (mask == mskVMTRef) {
         mssg_t message = encodeMessage(dummyModule->mapAction(CONSTRUCTOR_MESSAGE, 0, false), 
            p.value2, FUNCTION_MESSAGE);

         ByteCodeUtil::write(writer, ByteCode::SetR, p.value1);
         ByteCodeUtil::write(writer, ByteCode::MovM, message);
         ByteCodeUtil::write(writer, ByteCode::CallVI);
      }
      else {
         switch (p.value2) {
            case VM_ALLOC_CMD:
               ByteCodeUtil::write(writer, ByteCode::XFlushSI, 0);
               ByteCodeUtil::write(writer, ByteCode::XFlushSI, 1);
               ByteCodeUtil::write(writer, ByteCode::AllocI, p.value1);
               break;
            case VM_FREE_CMD:
               ByteCodeUtil::write(writer, ByteCode::FreeI, p.value1);
               ByteCodeUtil::write(writer, ByteCode::XRefreshSI, 0);
               ByteCodeUtil::write(writer, ByteCode::XRefreshSI, 1);
               break;
            case VM_SET_ARG_CMD:
               ByteCodeUtil::write(writer, ByteCode::StoreSI, p.value1);
               break;
            case VM_SEND_MESSAGE_CMD:
               ByteCodeUtil::write(writer, ByteCode::MovM, p.value1);
               ByteCodeUtil::write(writer, ByteCode::CallVI);
               break;
            default:
               break;
         }
      }
   }

   ByteCodeUtil::write(writer, ByteCode::CloseN);
   ByteCodeUtil::write(writer, ByteCode::Quit);

   pos_t size = writer.position() - sizePlaceholder - sizeof(pos_t);

   writer.seek(sizePlaceholder);
   writer.writePos(size);

   return true;
}

void ELENAVMMachine :: onNewCode(JITLinker& jitLinker)
{
   ustr_t superClass = !_startUpCode ? nullptr : _configuration->resolveForward(SUPER_FORWARD);

   jitLinker.complete(_compiler, superClass);

   _startUpCode = false;

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

addr_t ELENAVMMachine :: interprete(SystemEnv* env, void* tape, pos_t size, 
   const char* criricalHandlerReference, bool withConfiguration)
{
   ByteArray      tapeArray(tape, size);
   MemoryReader   reader(&tapeArray);

   stopVM();

   JITLinker* jitLinker = nullptr;

   Module* dummyModule = new Module();
   MemoryDump tapeSymbol;

   addr_t criricalHandler = 0;
   if (!withConfiguration || configurateVM(reader, env)) {
      jitLinker = new JITLinker(&_mapper, &_libraryProvider, _configuration, dynamic_cast<ImageProviderBase*>(this),
         &_settings, nullptr);

      if (!_initialized) {
         init(*jitLinker, env);

         criricalHandler = jitLinker->resolve(criricalHandlerReference, mskProcedureRef, false);

         if (!_standAloneMode)
            env = _env;
      }
      else jitLinker->setCompiler(_compiler);
   }

   if (compileVMTape(reader, tapeSymbol, *jitLinker, dummyModule)) {
      void* address = (void*)jitLinker->resolveTemporalByteCode(tapeSymbol, dummyModule);

      resumeVM(*jitLinker, env, (void*)criricalHandler);

      freeobj(dummyModule);
      freeobj(jitLinker);

      return execute(env, address);
   }
   if (!_standAloneMode) {
      resumeVM(*jitLinker, env, (void*)criricalHandler);
   }

   return 0;
}

void ELENAVMMachine :: startSTA(SystemEnv* env, void* tape, const char* criricalHandlerReference)
{
   if (tape != nullptr) {
      interprete(env, tape, INVALID_POS, criricalHandlerReference, true);
   }
   else {
      // initialize VM in terminal mode
      _standAloneMode = false;

      _settings.jitSettings.ygSize = env->gc_yg_size;
      _settings.jitSettings.mgSize = env->gc_mg_size;
      _settings.jitSettings.stackReserved = _settings.jitSettings.stackReserved;
   }
}

addr_t ELENAVMMachine :: evaluate(void* tape)
{
   return interprete(_env, tape, INVALID_POS, nullptr, false);
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
   JITLinker* jitLinker = new JITLinker(&_mapper, &_libraryProvider, _configuration, dynamic_cast<ImageProviderBase*>(this),
      &_settings, nullptr);

   jitLinker->setCompiler(_compiler);

   ustr_t name = jitLinker->retrieveResolvedAction(subjectRef);

   actionName.copy(name);

   delete jitLinker;
}

size_t ELENAVMMachine :: loadMessageName(mssg_t message, char* buffer, size_t length)
{
   ref_t actionRef, flags;
   pos_t argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);

   IdentifierString actionName;
   loadSubjectName(actionName, actionRef);

   IdentifierString messageName;
   ByteCodeUtil::formatMessageName(messageName, nullptr, *actionName, nullptr, 0, argCount, flags);

   StrConvertor::copy(buffer, *messageName, messageName.length(), length);

   return length;
}

size_t ELENAVMMachine :: loadAddressInfo(addr_t retPoint, char* lineInfo, size_t length)
{
   RTManager rtmanager(nullptr, getTargetDebugSection());

   return rtmanager.retriveAddressInfo(_libraryProvider, retPoint, lineInfo, length, true);
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
   if (!emptystr(name))
      addVMTapeEntry(tapeWriter, command, name);
   addVMTapeEntry(tapeWriter, VM_ENDOFTAPE_CMD);

   interprete(_env, tape.get(0), tape.length(), nullptr, true);

   return emptystr(name)
      ? 0 : _mapper.resolveReference({ nullptr, name }, getCmdMask(command));
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

mssg_t ELENAVMMachine :: loadAction(ustr_t actionName)
{
   pos_t argCount = 0;
   ref_t flags = 0;

   ref_t actionRef = loadSubject(actionName);
   if (!actionRef)
      return 0;

   return encodeMessage(actionRef, argCount, flags);
}

size_t ELENAVMMachine :: loadActionName(mssg_t message, char* buffer, size_t length)
{
   ref_t actionRef, flags;
   pos_t argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);

   IdentifierString actionName;
   loadSubjectName(actionName, actionRef);

   StrConvertor::copy(buffer, *actionName, actionName.length(), length);

   return length;
}

addr_t ELENAVMMachine :: loadSymbol(ustr_t name)
{
   addr_t addr = loadReference(name, VM_TRYCALLSYMBOL_CMD);

   return addr != INVALID_ADDR ? addr : 0;
}

bool ELENAVMMachine :: loadModule(ustr_t ns)
{
   auto retVal = _libraryProvider.loadModuleIfRequired(ns);
   if (retVal == LibraryProvider::ModuleRequestResult::Loaded) {
      if (_preloadedList.count() > 0) {
         loadReference(nullptr, 0);

         return true;
      }
   }
   else if (retVal == LibraryProvider::ModuleRequestResult::NotFound)
      return false;

   return false;
}

addr_t ELENAVMMachine :: retrieveGlobalAttribute(int attribute, ustr_t name)
{
   IdentifierString currentName;

   MemoryReader reader(getADataSection());
   pos_t size = reader.getDWord();

   pos_t pos = reader.position();
   while (pos < size) {
      int current = reader.getDWord();
      pos_t offset = reader.getDWord() + sizeof(addr_t);

      if (current == attribute) {
         reader.readString(currentName);
         if (currentName.compare(name)) {
            addr_t address = 0;
            reader.read(&address, sizeof(address));

            return address;
         }
      }

      pos += offset;
      reader.seek(pos);
   }

   return 0;
}

addr_t ELENAVMMachine :: loadDispatcherOverloadlist(ustr_t referenceName)
{
   return retrieveGlobalAttribute(GA_EXT_OVERLOAD_LIST, referenceName);
}

int ELENAVMMachine :: loadExtensionDispatcher(const char* moduleList, mssg_t message, void* output)
{
   // load message name
   char messageName[IDENTIFIER_LEN];
   size_t mssgLen = loadMessageName(message | FUNCTION_MESSAGE, messageName, IDENTIFIER_LEN);
   messageName[mssgLen] = 0;

   int len = 1;
   ((addr_t*)output)[0] = message;

   // search message dispatcher
   IdentifierString messageRef;
   size_t listLen = getlength(moduleList);
   size_t i = 0;
   while (moduleList[i]) {
      ustr_t ns = moduleList + i;

      loadModule(ns);

      messageRef.copy(ns);
      messageRef.append('\'');
      messageRef.append(messageName);

      addr_t listRef = loadDispatcherOverloadlist(*messageRef);
      if (listRef) {
         ((addr_t*)output)[len] = listRef;
         len++;
      }

      i += getlength(ns) + 1;
   }

   return len;
}

void ELENAVMMachine :: onLoad(ModuleBase* module)
{
   if (!_preloadedSection.empty()) {
      IdentifierString nameToResolve(META_PREFIX, *_preloadedSection);

      _libraryProvider.loadDistributedSymbols(module, *nameToResolve, _preloadedList);
   }
}

size_t ELENAVMMachine :: loadClassMessages(void* classPtr, mssg_t* output, size_t skip, size_t maxLength)
{
   MemoryBase* msection = getMDataSection();

   return SystemRoutineProvider::LoadMessages(msection, classPtr, output, 
      skip, maxLength, true);
}

bool ELENAVMMachine :: checkClassMessage(void* classPtr, mssg_t message)
{
   MemoryBase* msection = getMDataSection();

   return SystemRoutineProvider::CheckMessage(msection, classPtr, message);
}