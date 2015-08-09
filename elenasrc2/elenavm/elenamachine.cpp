//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenamachine.h"
#include "bytecode.h"
#include "rtman.h"

#include <stdarg.h>

using namespace _ELENA_;

#define NMODULE_LEN getlength(NATIVE_MODULE)

#define PROJECT_CATEGORY            "project"
#define SYSTEM_CATEGORY             "system"
#define LINKER_CATEGORY             "linker"
#define LIBRARY_CATEGORY            "library"
#define TEMPLATE_CATEGORY           "templates"
#define PRIMITIVE_CATEGORY          "primitives"
#define FORWARD_CATEGORY            "forwards"

#define PROJECT_TEMPLATE            "template"
//#define SYSTEM_MAXTHREAD            _T("maxthread")
#define LINKER_MGSIZE               "mgsize"
#define LINKER_OBJSIZE              "objsize"
#define LINKER_YGSIZE               "ygsize"
#define LIBRARY_PATH                "path"

// --- Wrapper Functions ---

//void* __getClassVMTRef(Instance* instance, void* referenceName)
//{
//   return instance->getClassVMTRef((const wchar16_t*)referenceName);
//}

void* __getSymbolRef(Instance* instance, void* referenceName)
{
   return instance->getSymbolRef((ident_t)referenceName);
}

void* __getSubjectRef(Instance* instance, void* subjectName)
{
   ref_t subj_id = instance->getSubjectRef((ident_t)subjectName);

   return (void*)(MESSAGE_MASK | encodeMessage(subj_id, 0, 0));
}

static size_t __getClassName(Instance* instance, void* vmtAddress, ident_c* buffer, size_t maxLength)
{
   ident_t className = instance->getClassName(vmtAddress);
   size_t length = getlength(className);
   if (length > 0) {
      if (maxLength >= length) {
         StringHelper::copy(buffer, className, length, length);
      }
      else buffer[0] = 0;
   }

   return length;
}

static size_t __getSubjectName(Instance* instance, void* subjectRef, ident_c* buffer, size_t maxLength)
{
   size_t verb_id, subj_id;
   int param_count;
   decodeMessage((size_t)subjectRef, subj_id, verb_id, param_count);

   ident_t subjectName = instance->getSubject((ref_t)subj_id);
   size_t length = getlength(subjectName);
   if (length > 0) {
      if (maxLength >= length) {
         StringHelper::copy(buffer, subjectName, length, length);
      }
      else buffer[0] = 0;
   }

   return length;
}

static void* __interprete(Instance* instance, void* tape)
{
   return (void*)instance->interprete(tape, VM_INTERPRET);
}

static void* __getLastError(Instance* instance, void* retVal)
{
   ident_t error = instance->getStatus();
   if (!emptystr(error)) {
      size_t length = getlength(error);
      StringHelper::copy((ident_c*)retVal, error, length, length);
      ((ident_c*)retVal)[length] = 0;

      return retVal;
   }
   else return NULL;
}

static size_t __loadAddressInfo(Instance* instance, void* address, ident_c* buffer, size_t maxLength)
{
   if (instance->loadAddressInfo(address, buffer, maxLength)) {
      return maxLength;
   }
   else return 0;
}

// --- InstanceConfig ---

void InstanceConfig :: loadForwardList(IniConfigFile& config)
{
   ConfigCategoryIterator it = config.getCategoryIt(FORWARD_CATEGORY);
   while (!it.Eof()) {
      const char* key = it.key();
      const char* value = (const char*)*it;

      // if it is a wildcard
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

void InstanceConfig :: loadList(IniConfigFile& config, const char* category, const wchar_t* path, Map<ident_t, ident_c*>* list)
{
   ConfigCategoryIterator it = config.getCategoryIt(category);
   while (!it.Eof()) {
      const char* key = it.key();
      const char* value = (const char*)*it;

      if(emptystr(value))
         value = key;

      // add path if provided
      if (!emptystr(path)) {
         Path filePath(path);
         Path::combinePath(filePath, value);

         list->add(key, IdentifierString::clonePath(filePath));
      }
      else list->add(key, StringHelper::clone(value));

      it++;
   }
}

bool InstanceConfig :: load(path_t path, Templates* templates)
{
   IniConfigFile config;
   if (_ELENA_::emptystr(path) || !config.load(path, feUTF8)) {
      return false;
   }

   if (templates != NULL) {
      // load template
      IdentifierString projectTemplate(config.getSetting(PROJECT_CATEGORY, PROJECT_TEMPLATE));

      if (!_ELENA_::emptystr(projectTemplate)) {
         Path templatePath;
         Path::loadPath(templatePath, templates->get(projectTemplate));

         load(templatePath, templates);
      }
   }

   _ELENA_::Path configPath;
   configPath.copySubPath(path);

   // init config
   init(configPath, config);

   return true;
}

void InstanceConfig :: init(path_t configPath, IniConfigFile& config)
{
   // compiler options
   //maxThread = config.getIntSetting(SYSTEM_CATEGORY, SYSTEM_MAXTHREAD, maxThread);
   mgSize = config.getIntSetting(LINKER_CATEGORY, LINKER_MGSIZE, mgSize);
   ygSize = config.getIntSetting(LINKER_CATEGORY, LINKER_YGSIZE, ygSize);
   objSize = config.getIntSetting(LINKER_CATEGORY, LINKER_OBJSIZE, objSize);

   const char* path = config.getSetting(LIBRARY_CATEGORY, LIBRARY_PATH, NULL);
   if (!emptystr(path)) {
      libPath.copy(configPath);
      Path::combinePath(libPath, path);
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
   MemoryWriter writer(_instance->getTargetDebugSection());

   writer.writeDWord(_codeBase + position);
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

   _machine = machine;

   // init loader based on default machine config
   initLoader(_machine->config);

   _literalClass.copy(_config.forwards.get(WSTR_FORWARD));
   _characterClass.copy(_config.forwards.get(WCHAR_FORWARD));
   _intClass.copy(_config.forwards.get(INT_FORWARD));
   _realClass.copy(_config.forwards.get(REAL_FORWARD));
   _longClass.copy(_config.forwards.get(LONG_FORWARD));
   _msgClass.copy(_config.forwards.get(MESSAGE_FORWARD));
   _signClass.copy(_config.forwards.get(SIGNATURE_FORWARD));
   _verbClass.copy(_config.forwards.get(VERB_FORWARD));

   // init Run-Time API
   _loadClassName = __getClassName;
   _loadSymbolPtr = __getSymbolRef;
   _interprete    = __interprete;
   _getLastError  = __getLastError;
   _loadAddrInfo  = __loadAddressInfo;
   _loadSubject = __getSubjectRef;
   _loadSubjectName = __getSubjectName;
}

Instance :: ~Instance()
{
   freeobj(_linker);
   freeobj(_compiler);
}

ident_t Instance :: resolveForward(ident_t forward)
{
   ident_t reference = _config.forwards.get(forward);
   // if no forward mapping was found try to resolve on the module level
   if (emptystr(reference)) {
      NamespaceName alias(forward);

      ident_t module = _config.moduleForwards.get(alias);
      // if there is a module mapping create an appropriate forward
      if (!emptystr(module)) {
         ReferenceName name(forward);
         ReferenceNs newRefeference(module, name);

         _config.forwards.add(forward, StringHelper::clone(newRefeference));

         reference = _config.forwards.get(forward);
      }
   }
   return reference;
}

size_t Instance :: getLinkerConstant(int id)
{
   switch (id) {
      case lnGCMGSize:
         return _config.mgSize;
      case lnGCYGSize:
         return _config.ygSize;
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
      case lnVMAPI_GetLastError:
         return (size_t)_getLastError;
      case lnVMAPI_LoadAddrInfo:
         return (size_t)_loadAddrInfo;
      case lnVMAPI_LoadSubjectName:
         return (size_t)_loadSubjectName;
      case lnVMAPI_LoadSubject:
         return (size_t)_loadSubject;
      default:
         return 0;
   }
}

void Instance :: printInfo(const wchar_t* msg, ...)
{
   va_list argptr;
   va_start(argptr, msg);

   vwprintf(msg, argptr);
   va_end(argptr);
   wprintf(L"\n");

   fflush(stdout);
}

ident_t Instance :: retrieveReference(_Module* module, ref_t reference, ref_t mask)
{
   if (mask == mskLiteralRef || mask == mskInt32Ref || mask == mskRealRef || mask == mskInt64Ref || mask == mskCharRef) {
      return module->resolveConstant(reference);
   }
   // if it is a message
   else if (mask == 0) {
      return module->resolveSubject(reference);
   }
   // if it is constant
   else {
      ident_t referenceName = module->resolveReference(reference);

      while (isWeakReference(referenceName)) {
         ident_t resolvedName = resolveForward(referenceName);
         if (!emptystr(resolvedName)) {
            referenceName = resolvedName;
         }
         else throw JITUnresolvedException(referenceName);
      }
      return referenceName;
   }
}

ident_t Instance :: retrieveReference(void* address, ref_t mask)
{
   if (mask == 0) {
      return retrieveKey(_subjects.start(), (ref_t)address, DEFAULT_STR);
   }
   else {
      switch (mask & mskImageMask) {
      case mskRDataRef:
         return retrieveKey(_dataReferences.start(), (ref_t)address, DEFAULT_STR);
      default:
         return NULL;
      }
   }
}

_Module* Instance::resolveModule(ident_t referenceName, LoadResult& result, ref_t& reference)
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

SectionInfo Instance::getSectionInfo(ident_t reference, size_t mask)
{
   SectionInfo sectionInfo;

   LoadResult result;
   ref_t      referenceID = 0;
   if (StringHelper::compare(reference, NATIVE_MODULE, NMODULE_LEN) && reference[NMODULE_LEN]=='\'') {
      sectionInfo.module = _loader.resolveNative(reference, result, referenceID);
   }
   else sectionInfo.module = resolveModule(reference, result, referenceID);

   sectionInfo.section = sectionInfo.module ? sectionInfo.module->mapSection(referenceID | mask, true) : NULL;

   if (sectionInfo.section == NULL) {
      throw JITUnresolvedException(reference);
   }

   return sectionInfo;
}

SectionInfo Instance :: getCoreSectionInfo(ref_t reference, size_t mask)
{
   SectionInfo sectionInfo;

   LoadResult result = lrNotFound;
   sectionInfo.module = _loader.resolveCore(reference, result);
   sectionInfo.section = sectionInfo.module ? sectionInfo.module->mapSection(reference | mask, true) : NULL;

   if (sectionInfo.section == NULL) {
      throw InternalError("Internal error");
   }

   return sectionInfo;
}

ClassSectionInfo Instance::getClassSectionInfo(ident_t reference, size_t codeMask, size_t vmtMask, bool silentMode)
{
   ClassSectionInfo sectionInfo;

   LoadResult result;
   ref_t      referenceID = 0;
   sectionInfo.module = resolveModule(reference, result, referenceID);

   if (sectionInfo.module == NULL || referenceID == 0) {
      if (!silentMode)
         throw JITUnresolvedException(reference);
   }
   else {
      sectionInfo.codeSection = sectionInfo.module->mapSection(referenceID | codeMask, true);
      sectionInfo.vmtSection = sectionInfo.module->mapSection(referenceID | vmtMask, true);
   }
   return sectionInfo;
}

void Instance::addForward(ident_t forward, ident_t reference)
{
   if (forward[getlength(forward) - 1] == '*') {
      NamespaceName alias(forward);
      NamespaceName module(reference);

      _config.moduleForwards.erase(alias);

      _config.moduleForwards.add(alias, StringHelper::clone(module));
   }
   else {
      _config.forwards.erase(forward);

      _config.forwards.add(forward, StringHelper::clone(reference));
   }
}

void Instance::addForward(ident_t line)
{
   size_t sep = StringHelper::find(line, '=', -1);
   if(sep != -1) {
      ident_t reference = line + sep + 1;
      IdentifierString forward(line, sep);

      addForward(forward, reference);
   }
}

void* Instance::loadSymbol(ident_t reference, int mask)
{
   // reference should not be a forward one
   while (isWeakReference(reference)) {
      ident_t resolved = resolveForward(reference);
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
      Path path;
      Path::loadPath(path, *it);
      if (StringHelper::compare(it.key(), CORE_ALIAS)) {
         _loader.addCoreAlias(path);
      }
      else _loader.addPrimitiveAlias(it.key(), path);

      it++;
   }

   return true;
}

bool Instance :: restart(bool debugMode)
{
   printInfo(ELENAVM_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELENAVM_REVISION_NUMBER);
   printInfo(L"Initializing...");

   clearReferences();

   // init debug section
   if (_linker->getDebugMode()) {
      printInfo(L"Debug mode...");
   }

   // load predefined code
   _linker->prepareCompiler();

   // initialize GC
   _Entry entry;
   entry.address = loadSymbol(VM_INIT, mskNativeCodeRef);

   (*entry.entry)();

   _initialized = true;

   printInfo(L"Done...");

   return true;
}

void Instance :: translate(MemoryReader& reader, ImageReferenceHelper& helper, MemoryDump& dump, int terminator)
{
   MemoryWriter ecodes(&dump);

   ecodes.writeDWord(0);            // write size place holder
   int procPtr = ecodes.Position();

   // open 0
   ecodes.writeByte(bcOpen);
   ecodes.writeDWord(0);

   // resolve tape
   size_t command = reader.getDWord();
   void*  extra_param;
   while (command != terminator) {
      ident_t arg = NULL;
      size_t param = reader.getDWord();
      if (test(command, LITERAL_ARG_MASK)) {
         arg = (ident_t)reader.Address();

         reader.seek(reader.Position() + param);  // goes to the next record
      }

      // in debug mode place a breakpoint excluding prefix command
      if (_debugMode)
         ecodes.writeByte(bcBreakpoint);

      switch(command) {
         case ARG_TAPE_MESSAGE_ID:
            extra_param = loadSymbol(arg, mskVMTRef);
            break;
         case CALL_TAPE_MESSAGE_ID: 
            //callr
            //pusha
            ecodes.writeByte(bcCallR);
            helper.writeTape(ecodes, loadSymbol(arg, mskSymbolRef), mskCodeRef);
            ecodes.writeByte(bcPushA);
            break;
         case PUSH_VAR_MESSAGE_ID:
            // pushfi param
            ecodes.writeByte(bcPushFI);
            ecodes.writeDWord(param);
            break;
         case ASSIGN_VAR_MESSAGE_ID:
            // popa
            // asavefi param
            ecodes.writeByte(bcPopA);
            ecodes.writeByte(bcASaveFI);
            ecodes.writeDWord(param);
            break;
         case PUSH_TAPE_MESSAGE_ID:
            //pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol(arg, mskConstantRef), mskRDataRef);
            //level++;

            break;
         case PUSHS_TAPE_MESSAGE_ID:
            //pushr constant
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol(arg, mskLiteralRef), mskRDataRef);
            break;
         case PUSHN_TAPE_MESSAGE_ID:
            //pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol(arg, mskInt32Ref), mskRDataRef);
            break;
         case PUSHR_TAPE_MESSAGE_ID:
            //pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol(arg, mskRealRef), mskRDataRef);
            break;
         case PUSHL_TAPE_MESSAGE_ID:
            //pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol(arg, mskInt64Ref), mskRDataRef);
            break;
         case PUSHM_TAPE_MESSAGE_ID:
            // pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol(arg, mskMessage), mskRDataRef);
            break;
         case PUSHG_TAPE_MESSAGE_ID:
            // pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol(arg, mskSignature), mskRDataRef);
            break;
         case POP_TAPE_MESSAGE_ID:
            //popi param
            ecodes.writeByte(bcPopI);
            ecodes.writeDWord(param);
            break;
         case REVERSE_TAPE_MESSAGE_ID:
            if (param == 2) {
               // popa
               // aswapsi 0
               // pusha
               ecodes.writeByte(bcPopA);
               ecodes.writeByte(bcASwapSI);
               ecodes.writeDWord(0);
               ecodes.writeByte(bcPushA);
            }
            else {
               int length = param >> 1;
               param--;

               // popa
               // aswapsi 0
               // pusha
               ecodes.writeByte(bcPopA);
               ecodes.writeByte(bcASwapSI);
               ecodes.writeDWord(param - 1);
               ecodes.writeByte(bcPushA);

               for (int i = 1 ; i < length ; i++) {
                  // aloadsi i
                  // aswapsi n - 1 - i
                  ecodes.writeByte(bcALoadSI);
                  ecodes.writeDWord(i);
                  ecodes.writeByte(bcASwapSI);
                  ecodes.writeDWord(param - i);
               }
            }
            break;
         case SEND_TAPE_MESSAGE_ID:
            //copym message
            //aloadsi 0
            //acallvi 0
            //pusha

            ecodes.writeByte(bcCopyM);
            ecodes.writeDWord(_linker->parseMessage(arg));
            ecodes.writeByte(bcALoadSI);
            ecodes.writeDWord(0);
            ecodes.writeByte(bcACallVI);
            ecodes.writeDWord(0);
            ecodes.writeByte(bcPushA);

            break;
         case NEW_TAPE_MESSAGE_ID:
         {
            int level = param;

            // new n, vmt
            ecodes.writeByte(bcNew);
            helper.writeTape(ecodes, extra_param, mskVMTRef);
            ecodes.writeDWord(param);

            // ; assign content
            // bcopya

            // ; repeat param-time
            // popa
            // axsavebi i

            // pushb
            ecodes.writeByte(bcBCopyA);
            while (level > 0) {
               ecodes.writeByte(bcPopA);
               ecodes.writeByte(bcAXSaveBI);
               level--;
               ecodes.writeDWord(level);
            }

            ecodes.writeByte(bcPushB);
            break;
         }
      }
      command = reader.getDWord();
   }
   // EOP breakpoint
   if (_debugMode)
      ecodes.writeByte(bcBreakpoint);

   // popa
   // close
   // quit
   ecodes.writeByte(bcPopA);
   ecodes.writeByte(bcClose);
   ecodes.writeByte(bcQuit);

   dump[procPtr - 4] = ecodes.Position() - procPtr;
}

bool Instance::loadTemplate(ident_t name)
{
   Path path;
   Path::loadPath(path, _machine->templates.get(name));

   if (!_config.load(path, &_machine->templates)) {
      setStatus("Cannot load the template:", name);

      return false;
   }

   return initLoader(_config);
}

void Instance::setPackagePath(ident_t package, path_t path)
{
   _loader.setPackage(package, path);
}

void Instance::setPackagePath(ident_t line)
{
   size_t sep = StringHelper::find(line, '=', -1);
   if(sep != -1) {
      Path path;
      Path::loadPath(path, line + sep + 1);
      IdentifierString package(line, sep);

      setPackagePath(package, path);
   }
   else {
      Path path;
      Path::loadPath(path, line);

      setPackagePath(NULL, path);
   }
}

void Instance :: configurate(MemoryReader& reader, int terminator)
{
   size_t pos = reader.Position();

   size_t command = reader.getDWord();
   while (command != terminator) {
      ident_t arg = NULL;
      size_t param = reader.getDWord();
      if (test(command, LITERAL_ARG_MASK)) {
         arg = (ident_t)reader.Address();

         reader.seek(reader.Position() + param);  // goes to the next record
      }

      switch (command) {
         case USE_VM_MESSAGE_ID:
            setPackagePath(arg);
            break;
         case MAP_VM_MESSAGE_ID:
            addForward(arg);
            break;
         case LOAD_VM_MESSAGE_ID:
            if(!loadTemplate(arg))
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

int Instance::interprete(void* tape, ident_t interpreter)
{
   ByteArray    tapeArray(tape, -1);
   MemoryReader tapeReader(&tapeArray);

   stopVM();

   // configurate VM instance
   configurate(tapeReader, 0);

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
   translate(tapeReader, helper, ecodes, 0);

   // compile byte code
   MemoryReader reader(&ecodes);

   void* vaddress = _linker->resolveTemporalByteCode(helper, reader, TAPE_SYMBOL, (void*)tape);

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
      setStatus("Broken");

   return retVal;
}

bool Instance :: loadAddressInfo(void* address, ident_c* buffer, size_t& maxLength)
{
   RTManager manager;
   MemoryReader reader(getTargetDebugSection(), 4);

   return manager.readAddressInfo(reader, (size_t)address, &_loader, buffer, maxLength);
}

// --- ELENAMachine::Config ---

bool ELENAMachine::Config :: load(path_t path, Templates* templates)
{
   IniConfigFile config;
   _ELENA_::Path rootPath;

   rootPath.copySubPath(path);

   if (_ELENA_::emptystr(path) || !config.load(path, feUTF8)) {
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

ELENAMachine :: ELENAMachine(path_t rootPath)
   : templates(NULL, freestr), _rootPath(rootPath)
{
   Path configPath(rootPath);
   Path::combinePath(configPath, "elenavm.cfg");

   config.load(configPath, &templates);
}
