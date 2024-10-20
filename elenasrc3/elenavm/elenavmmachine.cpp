//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA VM declaration
//
//                                             (C)2021-2024, by Aleksey Rakov
//                                             (C)2021-2024, by ELENA-LANG Org
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

   loadBoolSetting(config, root, STRICT_TYPE_ENFORCING_PATH, ProjectOption::StrictTypeEnforcing);
   loadBoolSetting(config, root, JUMP_ALIGNMENT_PATH, ProjectOption::WithJumpAlignment);

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
   _settings.jitSettings.withAlignedJump = _configuration->BoolSetting(ProjectOption::WithJumpAlignment);
   _settings.virtualMode = false;
   _settings.alignment = codeAlignment;

   _libraryProvider.addListener(this);

   _compiler = jitCompilerFactory(&_libraryProvider, platform);

   _jitLinker = nullptr;
}

ustr_t ELENAVMMachine::getArchitectureName() {
#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)
    return "64-bit";
#else
    return "32-bit";
#endif
}

void ELENAVMMachine :: init(SystemEnv* exeEnv)
{
   assert(_initialized == false);
   ustr_t architecture = getArchitectureName();
   _presenter->printLine(ELENAVM_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELENAVM_REVISION_NUMBER, architecture);
   _presenter->printLine(ELENAVM_INITIALIZING);

   _configuration->initLoader(_libraryProvider);

   if (_standAloneMode) {
      _compiler->populatePreloaded(
         (uintptr_t)exeEnv->th_table,
         (uintptr_t)exeEnv->th_single_content);
   }

   _jitLinker = new JITLinker(&_mapper, &_libraryProvider, _configuration, dynamic_cast<ImageProviderBase*>(this),
      &_settings, nullptr);
   _jitLinker->setCompiler(_compiler);

   _jitLinker->prepare(_settings.jitSettings);

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
            return true;
            break;
         case VM_ENDOFTAPE_CMD:
            eop = true;
            break;
         case VM_PRELOADED_CMD:
            _preloadedSection.copy(strArg);
            break;
         default:
            break;
      }
   }

   return false;
}

void ELENAVMMachine :: fillPreloadedSymbols(MemoryWriter& writer, ModuleBase* dummyModule)
{
   ModuleInfoList symbolList({});
   for (auto it = _preloadedList.start(); !it.eof(); ++it) {
      _jitLinker->copyPreloadedMetaList(*it, symbolList, false);
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

bool ELENAVMMachine :: compileVMTape(MemoryReader& reader, MemoryDump& tapeSymbol, ModuleBase* dummyModule, bool withSystemStartUp)
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
            _jitLinker->resolve(strArg, getCmdMask(command), false);
            symbols.add({ dummyModule->mapReference(strArg) | getCmdMask(command), 0 });
            break;
         case VM_TRYCALLSYMBOL_CMD:
            if (_jitLinker->resolve(strArg, getCmdMask(command), true) != INVALID_ADDR) {
               symbols.add({ dummyModule->mapReference(strArg) | getCmdMask(command), 0 });
            }
            else return false;
            break;
         case VM_STRING_CMD:
            _jitLinker->resolve(strArg, getCmdMask(command), false);
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

   if (withSystemStartUp)
      ByteCodeUtil::write(writer, ByteCode::System, 4);

   ByteCodeUtil::write(writer, ByteCode::ExtOpenIN, 2, 0);

   fillPreloadedSymbols(writer, dummyModule);

   bool needtoFlush = true;
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
               if (needtoFlush) {
                  ByteCodeUtil::write(writer, ByteCode::XFlushSI, 0);
                  ByteCodeUtil::write(writer, ByteCode::XFlushSI, 1);
                  needtoFlush = false;
               }
               ByteCodeUtil::write(writer, ByteCode::AllocI, p.value1);
               break;
            case VM_FREE_CMD:
               ByteCodeUtil::write(writer, ByteCode::FreeI, p.value1);
               ByteCodeUtil::write(writer, ByteCode::XRefreshSI, 0);
               ByteCodeUtil::write(writer, ByteCode::XRefreshSI, 1);
               needtoFlush = true;
               break;
            case VM_SET_ARG_CMD:
               ByteCodeUtil::write(writer, ByteCode::StoreSI, p.value1);
               needtoFlush = true;
               break;
            case VM_SEND_MESSAGE_CMD:
               ByteCodeUtil::write(writer, ByteCode::MovM, p.value1);
               ByteCodeUtil::write(writer, ByteCode::CallVI);
               needtoFlush = true;
               break;
            default:
               break;
         }
      }
   }

   ByteCodeUtil::write(writer, ByteCode::ExtCloseN);
   ByteCodeUtil::write(writer, ByteCode::XQuit);

   pos_t size = writer.position() - sizePlaceholder - sizeof(pos_t);

   writer.seek(sizePlaceholder);
   writer.writePos(size);

   return true;
}

void ELENAVMMachine :: onNewCode()
{
   ustr_t superClass = !_startUpCode ? nullptr : _configuration->resolveForward(SUPER_FORWARD);

   _jitLinker->complete(_compiler, superClass);

   _startUpCode = false;

   _mapper.clearLazyReferences();
}

void ELENAVMMachine :: resumeVM(SystemEnv* env, void* criricalHandler)
{
   if (!_initialized)
      throw InternalError(errVMNotInitialized);

   onNewCode();

   if (criricalHandler)
      __routineProvider.InitSTAExceptionHandling(env, criricalHandler);
}

addr_t ELENAVMMachine :: interprete(SystemEnv* env, void* tape, pos_t size, 
   const char* criricalHandlerReference, bool withConfiguration, bool withSystemStartUp)
{
   ByteArray      tapeArray(tape, size);
   MemoryReader   reader(&tapeArray);

   stopVM();

   Module* dummyModule = new Module();
   MemoryDump tapeSymbol;

   addr_t criricalHandler = 0;
   if (withConfiguration && configurateVM(reader, env)) {
      if (!_initialized) {
         init(env);

         criricalHandler = _jitLinker->resolve(criricalHandlerReference, mskProcedureRef, false);

         if (!_standAloneMode)
            env = _env;
      }
   }

   void* address = nullptr;
   if (_initialized) {
      if (compileVMTape(reader, tapeSymbol, dummyModule, withSystemStartUp))
         address = (void*)_jitLinker->resolveTemporalByteCode(tapeSymbol, dummyModule);

      resumeVM(env, (void*)criricalHandler);
   }

   freeobj(dummyModule);
   
   if (address)
      return execute(address);

   return 0;
}

void ELENAVMMachine :: startSTA(SystemEnv* env, void* tape, const char* criricalHandlerReference)
{
   if (tape != nullptr) {
      interprete(env, tape, INVALID_POS, criricalHandlerReference, true, false);
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
   return interprete(_env, tape, INVALID_POS, nullptr, false, true);
}

bool ELENAVMMachine :: evaluateAndReturn(void* tape, char* output, size_t maxLength, size_t& copied)
{
   auto result = evaluate(tape);
   if (result) {
      return SystemRoutineProvider::CopyResult(result, output, maxLength, copied);
   }

   return false;
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
   ustr_t name = _jitLinker->retrieveResolvedAction(subjectRef);

   actionName.copy(name);
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

   interprete(_env, tape.get(0), tape.length(), nullptr, true, false);

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

   ref_t actionRef = _jitLinker->resolveAction(actionName);

   resumeVM(_env, nullptr);

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

int ELENAVMMachine :: loadSignature(mssg_t message, addr_t* output, pos_t maximalCount)
{
   RTManager rtmanager(getMDataSection(), nullptr);

   ref_t actionRef, flags;
   pos_t argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);

   if (test(message, FUNCTION_MESSAGE) || ((message & PREFIX_MESSAGE_MASK) == CONVERSION_MESSAGE)) {
      argCount = _min(maximalCount, argCount);
   }
   else argCount = _min(maximalCount, argCount - 1);

   if (rtmanager.loadSignature(actionRef, maximalCount, output)) {
      return argCount;
   }

   return 0;
}