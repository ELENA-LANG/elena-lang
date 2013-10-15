//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "bytecode.h"

#include <stdarg.h>

using namespace _ELENA_;

#define PMODULE_LEN getlength(PACKAGE_MODULE)

#define PROJECT_CATEGORY            _T("project")
#define SYSTEM_CATEGORY             _T("system")
#define LINKER_CATEGORY             _T("linker")
#define LIBRARY_CATEGORY            _T("library")
#define TEMPLATE_CATEGORY           _T("templates")
#define PRIMITIVE_CATEGORY          _T("primitives")
#define FORWARD_CATEGORY            _T("forwards")

#define PROJECT_TEMPLATE            _T("template")
//#define SYSTEM_MAXTHREAD            _T("maxthread")
#define LINKER_GCSIZE               _T("gcsize")
#define LINKER_OBJSIZE              _T("objsize")
#define LINKER_YGRATIO              _T("ygratio")
#define LIBRARY_PATH                _T("path")

// --- Wrapper Functions ---
//
//void* __getClassVMTRef(Instance* instance, void* referenceName)
//{
//   return instance->getClassVMTRef((const wchar16_t*)referenceName);
//}

void* __getSymbolRef(Instance* instance, void* referenceName)
{
   return instance->getSymbolRef((const wchar16_t*)referenceName);
}

static void* __getClassName(Instance* instance, void* vmtAddress)
{
   return (void*)instance->getClassName(vmtAddress);
}

static void* __interprete(Instance* instance, void* tape)
{
   return (void*)instance->interprete(tape, VM_INTERPRET);
}

static void* __getLastError(Instance* instance, void* retVal)
{
   const wchar16_t* error = instance->getStatus();
   if (!emptystr(error)) {
      int length = getlength(error);
      StringHelper::copy((wchar16_t*)retVal, error, length);
      ((wchar16_t*)retVal)[length] = 0;

      return retVal;
   }
   else return NULL;
}

// --- InstanceConfig ---

void InstanceConfig :: loadForwardList(IniConfigFile& config)
{
   String<wchar16_t, 100> key;

   ConfigCategoryIterator it = config.getCategoryIt(FORWARD_CATEGORY);
   while (!it.Eof()) {
      // copy line key
      key.copy(it.key());

      // if it is a wildcard
      String<wchar16_t, 100> value((_text_t*)*it);
      if (key[getlength(key) - 1] == '*') {
         NamespaceName alias(key);
         NamespaceName module(value);

         moduleForwards.erase(alias);
         moduleForwards.add(alias, StringHelper::clone(module));
      }
      else {
         forwards.erase(key);
         forwards.add(key, StringHelper::clone(value));
      }
      it++;
   }
}

void InstanceConfig :: loadList(IniConfigFile& config, const wchar_t* category, const wchar_t* path, Map<const wchar_t*, wchar_t*>* list)
{
   String<wchar16_t, 100> key;

   ConfigCategoryIterator it = config.getCategoryIt(category);
   while (!it.Eof()) {
      // copy line key
      key.copy(it.key());

      // copy value or key if the value is absent
      String<wchar16_t, 100> value((_text_t*)*it);
      if(value.isEmpty())
         value.copy(key);

      // add path if provided
      if (!emptystr(path)) {
         Path filePath(path, value);

         list->add(key, StringHelper::clone(filePath));
      }
      else list->add(key, StringHelper::clone(value));

      it++;
   }
}

bool InstanceConfig :: load(const _path_t* path, Templates* templates)
{
   IniConfigFile config;
   if (_ELENA_::emptystr(path) || !config.load(path, feAnsi)) {
      return false;
   }

   if (templates != NULL) {
      // load template
      IdentifierString projectTemplate(config.getSetting(PROJECT_CATEGORY, PROJECT_TEMPLATE));

      if (!_ELENA_::emptystr(projectTemplate)) {
         load(templates->get(projectTemplate), templates);
      }
   }

   _ELENA_::Path configPath;
   configPath.copyPath(path);

   // init config
   init(configPath, config);

   return true;
}

void InstanceConfig :: init(const _path_t* configPath, IniConfigFile& config)
{
   // compiler options
   //maxThread = config.getIntSetting(SYSTEM_CATEGORY, SYSTEM_MAXTHREAD, maxThread);
   pageSize = config.getIntSetting(LINKER_CATEGORY, LINKER_GCSIZE, pageSize);
   ygRatio = config.getIntSetting(LINKER_CATEGORY, LINKER_YGRATIO, ygRatio);
   objSize = config.getIntSetting(LINKER_CATEGORY, LINKER_OBJSIZE, objSize);

   const _text_t* path = config.getSetting(LIBRARY_CATEGORY, LIBRARY_PATH, NULL);
   if (!emptystr(path)) {
      libPath.copy(configPath);
      libPath.combine(path);
   }

   loadList(config, PRIMITIVE_CATEGORY, configPath, &primitives);
   loadForwardList(config);
}

// --- Instance::ImageReferenceHelper ---

void Instance::ImageReferenceHelper :: writeTape(MemoryWriter& tape, void* vaddress, int mask)
{
   int ref = (size_t)vaddress - (test(mask, mskRDataRef) ? _statBase : _codeBase);

   tape.writeDWord(ref | mask);
}

void Instance::ImageReferenceHelper :: writeReference(MemoryWriter& writer, ref_t reference, size_t disp, _Module* module)
{
   size_t pos = reference & ~mskAnyRef;
   if (test(reference, mskRelCodeRef)) {
      writer.writeDWord(pos - writer.Position() - 4);
   }
   else writer.writeDWord((test(reference, mskRDataRef) ? _statBase : _codeBase) + pos + disp);
}

void Instance::ImageReferenceHelper :: writeReference(MemoryWriter& writer, void* vaddress, bool relative, size_t disp)
{
   ref_t address = (ref_t)vaddress;

   // calculate relative address
   if (relative)
      address -= ((ref_t)writer.Address() + 4);

   writer.writeDWord(address + disp);
}

void Instance::ImageReferenceHelper :: addBreakpoint(size_t position)
{
   if (_instance->_debugMode) {
      MemoryWriter writer(_instance->getTargetDebugSection());

      writer.writeDWord(_codeBase + position);
   }
}

// --- Instance ---

Instance :: Instance(ELENAMachine* machine)
   : _config(machine->config)
{
   _linker = NULL;
   _compiler = NULL;

   _initialized = false;
   _debugMode = false;

//   _traceMode = traceMode;

   _literalClass.copy(WSTR_CLASS);
   _intClass.copy(INT_CLASS);
   _realClass.copy(REAL_CLASS);
   _longClass.copy(LONG_CLASS);

   _machine = machine;

   // init loader based on default machine config
   initLoader(_machine->config);

   // init VM API
   _loadClassName = __getClassName;
   _loadSymbolPtr = __getSymbolRef;
   _interprete    = __interprete;
   _getLastError  = __getLastError;
}

Instance :: ~Instance()
{
   freeobj(_linker);
   freeobj(_compiler);
}

const wchar16_t* Instance :: resolveForward(const wchar16_t* forward)
{
   const wchar16_t* reference = _config.forwards.get(forward);
   // if no forward mapping was found try to resolve on the module level
   if (emptystr(reference)) {
      NamespaceName alias(forward);

      const wchar16_t* module = _config.moduleForwards.get(alias);
      // if there is a module mapping create an appropriate forward
      if (!emptystr(module)) {
         ReferenceName name(forward);
         ReferenceNs newRefeference(module, name);

         _config.forwards.add(forward, wcsdup(newRefeference));

         reference = _config.forwards.get(forward);
      }
   }
   return reference;
}

size_t Instance :: getLinkerConstant(int id)
{
   switch (id) {
      case lnGCSize:
         return _config.pageSize;
      case lnYGRatio:
         return _config.ygRatio;
      //case lnThreadCount:
      //   return (size_t)_config.maxThread;
      case lnObjectSize:
         return (size_t)_config.objSize;
      case lnVMAPI_Instance:
         return (size_t)this;
      case lnVMAPI_LoadSymbol:
         return (size_t)_loadSymbolPtr;
      case lnVMAPI_LoadName:
         return (size_t)_loadClassName;
      case lnVMAPI_Interprete:
         return (size_t)_interprete;
      //case lnVMAPI_GetLastError:
      //   return (size_t)_getLastError;
      default:
         return 0;
   }
}

void Instance :: printInfo(const _text_t* msg, ...)
{
   va_list argptr;
   va_start(argptr, msg);

   vwprintf(msg, argptr);
   va_end(argptr);
   wprintf(_T("\n"));

   fflush(stdout);
}

const wchar16_t* Instance :: retrieveReference(_Module* module, ref_t reference, ref_t mask)
{
   if (mask == mskLiteralRef || mask == mskInt32Ref || mask == mskRealRef || mask == mskInt64Ref) {
      return module->resolveConstant(reference);
   }
   // if it is a message
   else if (mask == 0) {
      return module->resolveSubject(reference);
   }
   // if it is constant
   else {
      const wchar16_t* referenceName = module->resolveReference(reference);
      while (isWeakReference(referenceName)) {
         const wchar16_t* resolvedName = resolveForward(referenceName);
         if (!emptystr(resolvedName)) {
            referenceName = resolvedName;
         }
         else throw JITUnresolvedException(referenceName);
      }
      return referenceName;
   }
}

const wchar16_t* Instance :: retrieveReference(void* address, ref_t mask)
{
   switch(mask & mskImageMask) {
      case mskRDataRef:
         return retrieveKey(_dataReferences.start(), (ref_t)address, (const wchar16_t*)NULL);
      default:
         return NULL;
   }
}

_Module* Instance :: resolveModule(const wchar16_t* referenceName, LoadResult& result, ref_t& reference)
{
   while (isWeakReference(referenceName)) {
      referenceName = resolveForward(referenceName);
   }
   if (emptystr(referenceName)) {
      result = lrNotFound;

      return NULL;
   }
   return _loader.resolveModule(referenceName, result, reference);
}

SectionInfo Instance :: getSectionInfo(const wchar16_t* reference, size_t mask)
{
   SectionInfo sectionInfo;

   LoadResult result;
   ref_t      referenceID = 0;
   if (ConstIdentifier::compare(reference, PACKAGE_MODULE, PMODULE_LEN)) {
      sectionInfo.module = _loader.resolvePrimitive(reference, result, referenceID);
   }
   else sectionInfo.module = resolveModule(reference, result, referenceID);
   sectionInfo.section = sectionInfo.module ? sectionInfo.module->mapSection(referenceID | mask, true) : NULL;

   if (sectionInfo.section == NULL) {
      throw JITUnresolvedException(reference);
   }

   return sectionInfo;
}

ClassSectionInfo Instance :: getClassSectionInfo(const wchar16_t* reference, size_t codeMask, size_t vmtMask)
{
   ClassSectionInfo sectionInfo;

   LoadResult result;
   ref_t      referenceID = 0;
   sectionInfo.module = resolveModule(reference, result, referenceID);
   if (sectionInfo.module != NULL && referenceID != 0) {
      sectionInfo.codeSection = sectionInfo.module->mapSection(referenceID | codeMask, true);
      sectionInfo.vmtSection = sectionInfo.module->mapSection(referenceID | vmtMask, true);
   }

   if (sectionInfo.codeSection == NULL) {
      throw JITUnresolvedException(reference);
   }

   return sectionInfo;
}

void Instance :: addForward(const wchar16_t* forward, const wchar16_t* reference)
{
   if (forward[getlength(forward) - 1] == '*') {
      NamespaceName alias(forward);
      NamespaceName module(reference);

      _config.moduleForwards.erase(alias);

      _config.moduleForwards.add(alias, StringHelper::cloneLowered(module));
   }
   else {
      _config.forwards.erase(forward);

      _config.forwards.add(forward, wcsdup(reference));
   }
}

void Instance :: addForward(const wchar16_t* line)
{
   size_t sep = StringHelper::find(line, '=', -1);
   if(sep != -1) {
      const wchar16_t* reference = line + sep + 1;
      IdentifierString forward(line, sep);

      addForward(forward, reference);
   }
}

void* Instance :: loadSymbol(const wchar16_t* reference, int mask)
{
   // reference should not be a forward one
   while (isWeakReference(reference)) {
      const wchar16_t* resolved = resolveForward(reference);
      if (emptystr(resolved)) {
         throw JITUnresolvedException(reference);
      }
      else reference = resolved;
   }
   return _linker->resolve(reference, mask, true);
}

bool Instance :: initLoader(InstanceConfig& config)
{
   // load paths
   _loader.setRootPath(config.libPath);

   // load primitives
   Primitives::Iterator it = config.primitives.start();
   while (!it.Eof()) {
      _loader.addPrimitiveAlias(it.key(), *it);

      it++;
   }

   return true;
}

bool Instance :: restart(bool debugMode)
{
   printInfo(ELENAVM_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELENAVM_BUILD_NUMBER);
   printInfo(_T("Initializing..."));

   clearReferences();

   // init debug section
   if (_linker->getDebugMode()) {
      printInfo(_T("Debug mode..."));

      // put size and entry point place holders
      int dummy = 0;
      getTargetDebugSection()->write(0, &dummy, 4);
      getTargetDebugSection()->write(4, &dummy, 4);
   }

   // load core modules
   LoadResult result;
   _Module* core = _loader.loadPrimitive(ConstantIdentifier(CORE_MODULE), result);
   if (result != lrSuccessful) {
      setStatus(_T("Cannot load "), ConstantIdentifier(CORE_MODULE));

      return false;
   }

   _Module* commands = _loader.loadPrimitive(ConstantIdentifier(COMMAND_MODULE), result);
   if (result != lrSuccessful) {
      setStatus(_T("Cannot load "), ConstantIdentifier(COMMAND_MODULE));

      return false;
   }

   // load predefined code
   _linker->prepareCompiler(core, commands);

   // initialize GC
   _Entry entry;
   entry.address = loadSymbol(VM_INIT, mskNativeCodeRef);

   (*entry.entry)();

   _initialized = true;

   printInfo(_T("Done..."));

   return true;
}

inline void reverseArgOrder(MemoryWriter& ecodes, int count, bool useRole)
{
   if (useRole) {
      ecodes.writeByte(bcPushAcc);

      int j = count;
      for(int i = 1 ; i <= count >> 1 ; i++) {
         // accloadsi j
         // accswapssi i
         // accsavesi j
         ecodes.writeByte(bcAccLoadSI);
         ecodes.writeDWord(j);
         ecodes.writeByte(bcAccSwapSI);
         ecodes.writeDWord(i);
         ecodes.writeByte(bcAccSaveSI);
         ecodes.writeDWord(j);
         j--;
      }
      ecodes.writeByte(bcPopAcc);
   }
   else {
      int j = count - 1;
      for(int i = 0 ; i < count >> 1 ; i++) {
         if (i == 0) {
            // swapsi j
            ecodes.writeByte(bcSwapSI);
            ecodes.writeDWord(j);
         }
         else {
            // accloadsi j
            // accswapssi i
            // accsavesi j
            ecodes.writeByte(bcAccLoadSI);
            ecodes.writeDWord(j);
            ecodes.writeByte(bcAccSwapSI);
            ecodes.writeDWord(i);
            ecodes.writeByte(bcAccSaveSI);
            ecodes.writeDWord(j);
         }
         j--;
      }
   }
}

void Instance :: translate(size_t base, MemoryReader& reader, ImageReferenceHelper& helper, MemoryDump& dump, int terminator)
{
   MemoryWriter ecodes(&dump);

   ecodes.writeDWord(0);            // write size place holder
   int procPtr = ecodes.Position();

   // open 0
   ecodes.writeByte(bcOpen);
   ecodes.writeDWord(0);

   // resolve tape
   bool useRole = false;
   int level = 0;
   int marker = 0;
   size_t message = 0;
   size_t command = reader.getDWord();
   while (command != terminator) {
      size_t param = reader.getDWord();

      reader.seek(reader.getDWord());  // goes to the next record

      // in debug mode place a breakpoint excluding prefix command
      if (_debugMode)
         ecodes.writeByte(bcBreakpoint);

      switch(command) {
         case START_TAPE_MESSAGE_ID:
            marker = level;
            break;
         case CALL_TAPE_MESSAGE_ID: 
            //callr
            //pushacc
            ecodes.writeByte(bcCallR);
            helper.writeTape(ecodes, loadSymbol((wchar16_t*)(base + param), mskSymbolRef), mskCodeRef);
            ecodes.writeByte(bcPushAcc);
            level++;

            break;
         case PUSH_EMPTY_MESSAGE_ID:
            // pushn 0
            ecodes.writeByte(bcPushN);
            ecodes.writeDWord(0);
            level++;
            break;
         case PUSH_VAR_MESSAGE_ID:
            // pushfi param
            ecodes.writeByte(bcPushFI);
            ecodes.writeDWord(param);
            level++;
            break;
         case POP_VAR_MESSAGE_ID:
            // popfi param
            ecodes.writeByte(bcPopFI);
            ecodes.writeDWord(param);
            level--;

            break;
         case PUSH_TAPE_MESSAGE_ID:
            //pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol((wchar16_t*)(base + param), mskConstantRef), mskRDataRef);
            level++;

            break;
         case PUSHS_TAPE_MESSAGE_ID:
            //pushr constant
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol((wchar16_t*)(base + param), mskLiteralRef), mskRDataRef);
            level++;
            break;
         case PUSHN_TAPE_MESSAGE_ID:
            //pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol((wchar16_t*)(base + param), mskInt32Ref), mskRDataRef);
            level++;

            break;
         case PUSHR_TAPE_MESSAGE_ID:
            //pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol((wchar16_t*)(base + param), mskRealRef), mskRDataRef);
            level++;

            break;
         case PUSHL_TAPE_MESSAGE_ID:
            //pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol((wchar16_t*)(base + param), mskInt64Ref), mskRDataRef);
            level++;

            break;
         case PUSHM_TAPE_MESSAGE_ID:
            // pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol((wchar16_t*)(base + param), mskMessage), mskRDataRef);
            level++;
            break;
         case PUSHB_TAPE_MESSAGE_ID:
            // pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol((wchar16_t*)(base + param), mskSymbolLoaderRef), mskRDataRef);
            level++;
            break;
         case POP_TAPE_MESSAGE_ID:
            //popn param
            ecodes.writeByte(bcPopN);
            ecodes.writeDWord(param);
            level -= param;
            break;
         case POP_ROLE_MESSAGE_ID:
            //popacc
            ecodes.writeByte(bcPopAcc);
            useRole = true;
            level--;
            break;
         case SEND_TAPE_MESSAGE_ID:
            message = _linker->parseMessage((wchar16_t*)(base + param));

            // reverse the parameter order
            if (getParamCount(message) > 0) 
               reverseArgOrder(ecodes, 1 + getParamCount(message), useRole);

            //mcccopym message
            //accloadsi 0
            //callacc 0
            //pushacc

            ecodes.writeByte(bcMccCopyM);
            ecodes.writeDWord(message);

            if (!useRole) {
               ecodes.writeByte(bcAccLoadSI);
               ecodes.writeDWord(0);
            }
            else useRole = false;

            ecodes.writeByte(bcCallAcc);
            ecodes.writeDWord(0);
            ecodes.writeByte(bcPushAcc);

            level -= getParamCount(message);

            break;
         case NEW_TAPE_MESSAGE_ID:
            // create n, vmt
            ecodes.writeByte(bcCreate);
            ecodes.writeDWord(level - marker);
            helper.writeTape(ecodes, loadSymbol((wchar16_t*)(base + param), mskVMTRef), mskVMTRef);

            // assign content
            while (level > marker) {
               ecodes.writeByte(bcXPopAccI);
               level--;
               ecodes.writeDWord(level - marker);
            }

            ecodes.writeByte(bcPushAcc);
            level++;

            break;
      }
      command = reader.getDWord();
   }
   // EOP breakpoint
   if (_debugMode)
      ecodes.writeByte(bcBreakpoint);

   // popacc
   // close
   // quit
   ecodes.writeByte(bcPopAcc);
   ecodes.writeByte(bcClose);
   ecodes.writeByte(bcQuit);

   dump[procPtr - 4] = ecodes.Position() - procPtr;
}

bool Instance :: loadTemplate(const wchar16_t* name)
{
   if (!_config.load(_machine->templates.get(name), &_machine->templates)) {
      setStatus(_T("Cannot load the template:"), name);

      return false;
   }

   return initLoader(_config);
}

void Instance :: setPackagePath(const wchar16_t* package, const _path_t* path)
{
   _loader.setPackage(package, path);
}

void Instance :: setPackagePath(const wchar16_t* line)
{
   size_t sep = StringHelper::find(line, '=', -1);
   if(sep != -1) {
      Path path(line + sep + 1);
      IdentifierString package(line, sep);

      setPackagePath(package, path);
   }
   else setPackagePath(NULL, line);
}

void Instance :: configurate(size_t base, MemoryReader& reader, int terminator)
{
   size_t pos = reader.Position();

   size_t command = reader.getDWord();
   while (command != terminator) {
      size_t param = reader.getDWord();

      reader.seek(reader.getDWord());  // goes to the next record
      switch (command) {
         case USE_VM_MESSAGE_ID:
            //printInfo(_T("package %s"), (wchar16_t*)(param + base));
            setPackagePath((wchar16_t*)(param + base));
            break;
         case MAP_VM_MESSAGE_ID:
            //printInfo(_T("mapping %s"), (wchar16_t*)(param + base));
            addForward((wchar16_t*)(param + base));
            break;
         case LOAD_VM_MESSAGE_ID:
            //printInfo(_T("load %s"),(wchar16_t*)(param + base));
            if(!loadTemplate((wchar16_t*)(param + base)))
               throw EAbortException();

            break;
         case START_VM_MESSAGE_ID:
            if(!restart(_debugMode))
               throw EAbortException();

            break;
         default:
            reader.seek(pos);
            return;
      }

      pos = reader.Position();
      command = reader.getDWord();
   }
   reader.seek(pos);
}

//void* Instance :: findDebugEntryPoint(ByteArray& tape)
//{
//   size_t base = (size_t)tape.get(0);
//   MemoryReader reader(&tape);
//
//   size_t command = reader.getDWord();
//   while (command != 0) {
//      size_t param = reader.getDWord();
//
//      reader.seek(reader.getDWord());  // goes to the next record
//      if (command == CALL_TAPE_MESSAGE_ID)
//         return (void*)resolveReference((TCHAR*)(base + param), mskSymbolRef);
//
//      command = reader.getDWord();
//   }
//   return NULL;
//}

int Instance :: interprete(void* tape, const wchar16_t* interpreter)
{
   size_t base = (size_t)tape;

   ByteArray    tapeArray(tape, -1);
   MemoryReader tapeReader(&tapeArray);

   stopVM();

   // configurate VM instance
   configurate(base, tapeReader, 0);

   if (!_initialized)
      throw InternalError("ELENAVM is not initialized");

   // exit if nothing to interprete
   if (tapeArray[tapeReader.Position()] == 0)
      return -1;

   // get dynamic symbol vaddress

   // !! probably, it is better to use jitlinker reference helper class
   ImageReferenceHelper helper(this);

   // lead symbol invoker
   _Entry entry;
   entry.address = loadSymbol(interpreter, mskNativeCodeRef);

   // create byte code tape
   MemoryDump   ecodes;
   translate(base, tapeReader, helper, ecodes, 0);

   // compile byte code
   MemoryReader reader(&ecodes);

   void* vaddress = _linker->resolveTemporalByteCode(helper, reader, ConstantIdentifier(TAPE_SYMBOL), (void*)base);

   // update debug section size if available
   if (_debugMode) {
      _Memory* debugSection = getTargetDebugSection();

      (*debugSection)[0] = debugSection->Length();
   }

   resumeVM();

   // raise an exception to warn debugger
   if(_debugMode)
      raiseBreakpoint();

   int retVal = (*entry.evaluate)(vaddress);

   if (retVal == 0)
      setStatus(_T("Broken"));

   return retVal;
}

// --- ELENAMachine::Config ---

bool ELENAMachine::Config :: load(const _path_t* path, Templates* templates)
{
   IniConfigFile config;
   _ELENA_::Path rootPath;

   rootPath.copyPath(path);

   if (_ELENA_::emptystr(path) || !config.load(path, feAnsi)) {
      return false;
   }

   // load templates
   if (templates) {
      loadList(config, TEMPLATE_CATEGORY, rootPath, templates);
   }

   init(rootPath, config);

   return true;
}

// --- ELENAMachine ---

ELENAMachine :: ELENAMachine(const _path_t* rootPath)
   : templates(NULL, freestr), _rootPath(rootPath)
{
   Path configPath(rootPath, _T("elenavm.cfg"));

   config.load(configPath, &templates);
}
