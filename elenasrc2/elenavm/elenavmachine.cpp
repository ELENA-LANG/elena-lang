//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA JIT Compiler Engine
//
//                                              (C)2009-2021, by Alexei Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenavmachine.h"
#include "bytecode.h"
#include "rtman.h"

#include <stdarg.h>

using namespace _ELENA_;

#define NMODULE_LEN getlength(NATIVE_MODULE)

#define TEMPLATE_CATEGORY           "configuration/templates/*"
#define PRIMITIVE_CATEGORY          "configuration/primitives/*"
#define FORWARD_CATEGORY            "configuration/forwards/*"

#define PROJECT_TEMPLATE            "configuration/project/template"
//#define SYSTEM_MAXTHREAD            _T("maxthread")
#define LINKER_MGSIZE               "configuration/linker/mgsize"
#define LINKER_YGSIZE               "configuration/linker/ygsize"
#define LINKER_PERMSIZE             "configuration/linker/permsize"
#define SYSTEM_PLATFORM             "configuration/system/platform"
#define LIBRARY_PATH                "configuration/project/libpath"

#if _WIN64

#define CONFIG_PATH "elenavm64.cfg"

#elif _WIN32

#define CONFIG_PATH "elenavm.cfg"

#elif _LINUX

#define CONFIG_PATH "/etc/elena/elenavm.config"

#endif // _WIN32

// --- InstanceConfig ---

void InstanceConfig :: loadForwardList(XmlConfigFile& config)
{
   _ConfigFile::Nodes nodes;
   config.select(FORWARD_CATEGORY, nodes);

   _ConfigFile::Nodes::Iterator it = nodes.start();
   while (!it.Eof()) {
      ident_t key = (*it).Attribute("key");
      ident_t value = (*it).Content();

      // if it is a wildcard
      if (key[getlength(key) - 1] == '*') {
         NamespaceName alias(key);
         NamespaceName module(value);

         moduleForwards.erase(alias);
         moduleForwards.add(alias, StrFactory::clone(module));
      }
      else {
         forwards.erase(key);
         forwards.add(key, StrFactory::clone(value));
      }
      it++;
   }
}

void InstanceConfig :: loadList(XmlConfigFile& config, const char* category, path_t path, Map<ident_t, char*>* list)
{
   _ConfigFile::Nodes nodes;
   config.select(category, nodes);

   _ConfigFile::Nodes::Iterator it = nodes.start();
   while (!it.Eof()) {
      ident_t key = (*it).Attribute("key");
      ident_t value = (*it).Content();

      if(emptystr(value))
         value = key;

      // add path if provided
      if (!emptystr(path)) {
         Path filePath(path);

         if (value[0] == '~') {
            filePath.copy(value + 1);
         }
         else filePath.combine(value);

         list->add(key, IdentifierString::clonePath(filePath.c_str()));
      }
      else list->add(key, StrFactory::clone(value));

      it++;
   }
}

bool InstanceConfig :: load(path_t path, Templates* templates)
{
   XmlConfigFile config;
   if (_ELENA_::emptystr(path) || !config.load(path, feUTF8)) {
      return false;
   }

   if (templates != NULL) {
      // load template
      IdentifierString projectTemplate(config.getSetting(PROJECT_TEMPLATE));

      if (!_ELENA_::emptystr(projectTemplate)) {
         Path templatePath(templates->get(projectTemplate));

         load(templatePath.c_str(), templates);
      }
   }

   _ELENA_::Path configPath;
   configPath.copySubPath(path);

   // init config
   init(configPath.c_str(), config);

   return true;
}

void InstanceConfig :: init(path_t configPath, XmlConfigFile& config)
{
   // compiler options
   //maxThread = config.getIntSetting(SYSTEM_CATEGORY, SYSTEM_MAXTHREAD, maxThread);
   mgSize = config.getHexSetting(LINKER_MGSIZE, mgSize);
   ygSize = config.getHexSetting(LINKER_YGSIZE, ygSize);
   permSize = config.getHexSetting(LINKER_PERMSIZE, permSize);
   platform = config.getIntSetting(SYSTEM_PLATFORM, platform);

   const char* path = config.getSetting(LIBRARY_PATH);
   if (!emptystr(path)) {
      libPath.copy(configPath);
      libPath.combine(path);
   }

   loadList(config, PRIMITIVE_CATEGORY, configPath, &primitives);
   loadForwardList(config);
}

// --- Instance::ImageReferenceHelper ---

void Instance::ImageReferenceHelper :: writeTape(MemoryWriter& tape, lvaddr_t vaddress, int mask)
{
   int ref = vaddress - (test(mask, mskRDataRef) ? _statBase : _codeBase);

   tape.writeDWord(ref | mask);
}

void Instance::ImageReferenceHelper :: writeReference(MemoryWriter& writer, ref_t reference, pos_t disp, _Module* module)
{
   size_t pos = reference & ~mskImageMask;
   if ((reference & mskImageMask) == mskRelCodeRef) {
      writer.writeDWord(pos - writer.Position() - 4);
   }
   else writer.writeDWord((test(reference, mskRDataRef) ? _statBase : _codeBase) + pos + disp);
}

void Instance::ImageReferenceHelper :: writeVAddress(MemoryWriter& writer, lvaddr_t vaddress, pos_t disp)
{
   ref_t address = (ref_t)vaddress;

   writer.writeDWord(address + disp);
}

void Instance::ImageReferenceHelper :: writeRelVAddress(MemoryWriter& writer, lvaddr_t vaddress, ref_t, pos_t disp)
{
   ref_t address = (ref_t)vaddress;

   // calculate relative address
   address -= ((ref_t)writer.Address() + 4);

   writer.writeDWord(address + disp);
}

void Instance::ImageReferenceHelper :: writeMTReference(MemoryWriter& writer)
{
   _Memory* section = _instance->getMessageSection();

   writer.writePtr((uintptr_t)section->get(0));
}

void Instance::ImageReferenceHelper :: addBreakpoint(pos_t position)
{
   MemoryWriter writer(_instance->getTargetDebugSection());

   writer.writeDWord(_codeBase + position);
}

// --- Instance ---

Instance :: Instance(ELENAVMMachine* machine)
   : _config(machine->config)
{
   _linker = NULL;
   _compiler = NULL;
   _messageTable = nullptr;
   _messageBodyTable = nullptr;
   _mattributeTable = nullptr;

   _initialized = false;
   _debugMode = false;
   _withExtDispatchers = /*false*/true;

//   _traceMode = traceMode;

   _machine = machine;

   // init loader based on default machine config
   initLoader(_machine->config);

   // create message table module
   _ConvertedMTSize = 0;
   _ConvertedMATSize = 0;
   LoadResult result = lrSuccessful;
   _Module* messages = _loader.createModule(META_MODULE, result);
   if (result == lrSuccessful) {
      _messageTable = messages->mapSection(messages->mapReference(MESSAGE_TABLE + getlength(META_MODULE)) | mskRDataRef, false);
      _messageBodyTable = messages->mapSection(messages->mapReference(MESSAGEBODY_TABLE + getlength(META_MODULE)) | mskRDataRef, false);
      _mattributeTable = messages->mapSection(messages->mapReference(MATTRIBUTE_TABLE + getlength(META_MODULE)) | mskRDataRef, false);

      clearMessageTable();
      clearMetaAttributeTable();
   }
   else throw EAbortException();
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

         _config.forwards.add(forward, StrFactory::clone(newRefeference));

         reference = _config.forwards.get(forward);
      }
   }
   return reference;
}

pos_t Instance :: getLinkerConstant(int id)
{
   switch (id) {
      case lnGCMGSize:
         return _config.mgSize;
      case lnGCYGSize:
         return _config.ygSize;
      case lnGCPERMSize:
         return _config.permSize;
         //case lnThreadCount:
      //   return (size_t)_config.maxThread;
      case lnObjectSize:
         return _compiler->getObjectHeaderSize();
      default:
         return 0;
   }
}

#ifdef _WIN32
void Instance :: printInfo(const wchar_t* msg, ...)
{
   va_list argptr;
   va_start(argptr, msg);

   vwprintf(msg, argptr);
   va_end(argptr);
   wprintf(L"\n");

   fflush(stdout);
}
#elif _LINUX

void Instance :: printInfo(const char* msg, ...)
{
   va_list argptr;
   va_start(argptr, msg);

   vprintf(msg, argptr);
   va_end(argptr);
   printf("\n");

   fflush(stdout);
}

#endif

ident_t Instance :: resolveTemplateWeakReference(ident_t referenceName)
{
   ident_t resolvedName = resolveForward(referenceName + TEMPLATE_PREFIX_NS_LEN);
   if (emptystr(resolvedName)) {
      if (referenceName.endsWith(CLASSCLASS_POSTFIX)) {
         // HOTFIX : class class reference should be resolved simultaneously with class one
         IdentifierString classReferenceName(referenceName, getlength(referenceName) - getlength(CLASSCLASS_POSTFIX));

         classReferenceName.copy(resolveTemplateWeakReference(classReferenceName.c_str()));
         classReferenceName.append(CLASSCLASS_POSTFIX);

         addForward(referenceName + TEMPLATE_PREFIX_NS_LEN, classReferenceName.c_str());

         return resolveForward(referenceName + TEMPLATE_PREFIX_NS_LEN);
      }

      // COMPILER MAGIC : try to find a template implementation
      ref_t resolvedRef = 0;
      _Module* refModule = resolveWeakModule(referenceName + TEMPLATE_PREFIX_NS_LEN, resolvedRef);
      if (refModule != nullptr) {
         ident_t resolvedReferenceName = refModule->resolveReference(resolvedRef);
         if (isWeakReference(resolvedReferenceName)) {
            IdentifierString fullName(refModule->Name(), resolvedReferenceName);

            addForward(referenceName + TEMPLATE_PREFIX_NS_LEN, fullName);
         }
         else addForward(referenceName + TEMPLATE_PREFIX_NS_LEN, resolvedReferenceName);

         referenceName = resolveForward(referenceName + TEMPLATE_PREFIX_NS_LEN);
      }
      else throw JITUnresolvedException(referenceName);
   }
   else referenceName = resolvedName;

   return referenceName;
}

ReferenceInfo Instance :: retrieveReference(_Module* module, ref_t reference, ref_t mask)
{
   if (mask == mskLiteralRef || mask == mskInt32Ref || mask == mskRealRef || mask == mskInt64Ref || mask == mskCharRef || mask == mskWideLiteralRef) {
      return module->resolveConstant(reference);
   }
   // if it is a message
   else if (mask == 0) {
      ref_t signRef = 0;
      return module->resolveAction(reference, signRef);
   }
   // if it is constant
   else {
      ident_t referenceName = module->resolveReference(reference);
      while (isForwardReference(referenceName)) {
         ident_t resolvedName = resolveForward(referenceName + FORWARD_PREFIX_NS_LEN);
         if (!emptystr(resolvedName)) {
            referenceName = resolvedName;
         }
         else throw JITUnresolvedException(referenceName);
      }

      if (isWeakReference(referenceName)) {
         if (isTemplateWeakReference(referenceName)) {
            referenceName = resolveTemplateWeakReference(referenceName);
         }

         return ReferenceInfo(module, referenceName);
      }
      return ReferenceInfo(referenceName);
   }
}

ident_t Instance :: retrieveReference(void* address, ref_t mask)
{
   //if (mask == 0) {
   //   return retrieveKey(_actions.start(), (ref_t)address, DEFAULT_STR);
   //}
   //else {
      switch (mask & mskImageMask) {
         case mskRDataRef:
            return retrieveKey(_dataReferences.start(), (ref_t)address, DEFAULT_STR);
         default:
            return nullptr;
      }
   //}
}

_Module* Instance :: resolveModule(ident_t referenceName, LoadResult& result, ref_t& reference)
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

_Module* Instance :: resolveWeakModule(ident_t weakReferenceName, ref_t& reference)
{
   LoadResult result = lrNotFound;
   _Module* module = _loader.resolveWeakModule(weakReferenceName, result, reference);
   if (result != lrSuccessful) {
      // Bad luck : try to resolve it indirectly
      module = _loader.resolveIndirectWeakModule(weakReferenceName, result, reference);
      if (result != lrSuccessful) {
         return NULL;
      }
      else return module;
   }
   else return module;
}

SectionInfo Instance :: getSectionInfo(ReferenceInfo referenceInfo, ref_t mask, bool silentMode)
{
   SectionInfo sectionInfo;
   LoadResult result;

   if (referenceInfo.isRelative()) {
      ref_t referenceID = referenceInfo.module->mapReference(referenceInfo.referenceName, true);

      sectionInfo.module = referenceInfo.module;
      sectionInfo.section = sectionInfo.module->mapSection(referenceID | mask, true);
   }
   else {
      ref_t      referenceID = 0;

      if (referenceInfo.referenceName.compare(NATIVE_MODULE, NMODULE_LEN) && referenceInfo.referenceName[NMODULE_LEN] == '\'') {
         sectionInfo.module = _loader.resolveNative(referenceInfo.referenceName, result, referenceID);
      }
      else sectionInfo.module = resolveModule(referenceInfo.referenceName, result, referenceID);

      sectionInfo.section = sectionInfo.module ? sectionInfo.module->mapSection(referenceID | mask, true) : NULL;
   }

   if (sectionInfo.section == NULL && !silentMode) {
      throw JITUnresolvedException(referenceInfo);
   }

   return sectionInfo;
}

SectionInfo Instance :: getCoreSectionInfo(ref_t reference, ref_t mask)
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

ClassSectionInfo Instance :: getClassSectionInfo(ReferenceInfo referenceInfo, ref_t codeMask, ref_t vmtMask, bool silentMode)
{
   ClassSectionInfo sectionInfo;

   ref_t referenceID = 0;
   LoadResult result;
   if (referenceInfo.isRelative()) {
      if (isTemplateWeakReference(referenceInfo.referenceName)) {
         sectionInfo.module = resolveModule(referenceInfo.referenceName, result, referenceID);
      }
      else {
         sectionInfo.module = referenceInfo.module;
         referenceID = referenceInfo.module->mapReference(referenceInfo.referenceName, true);
      }
   }
   else sectionInfo.module = resolveModule(referenceInfo.referenceName, result, referenceID);

   if (sectionInfo.module == NULL || referenceID == 0) {
      if (!silentMode)
         throw JITUnresolvedException(referenceInfo);
   }
   else {
      sectionInfo.codeSection = sectionInfo.module->mapSection(referenceID | codeMask, true);
      sectionInfo.vmtSection = sectionInfo.module->mapSection(referenceID | vmtMask, true);
      sectionInfo.attrSection = sectionInfo.module->mapSection(referenceID | mskAttributeRef, true);
   }

   return sectionInfo;
}

void Instance :: addForward(ident_t forward, ident_t reference)
{
   if (forward[getlength(forward) - 1] == '*') {
      NamespaceName alias(forward);
      NamespaceName module(reference);

      _config.moduleForwards.erase(alias);

      _config.moduleForwards.add(alias, StrFactory::clone(module));
   }
   else {
      _config.forwards.erase(forward);

      _config.forwards.add(forward, reference.clone());
   }
}

void Instance :: addForward(ident_t line)
{
   size_t sep = line.find('=', -1);
   if(sep != -1) {
      ident_t reference = line + sep + 1;
      IdentifierString forward(line, sep);

      addForward(forward, reference);
   }
}

void Instance :: onNewCode(SystemEnv* env)
{
   resolveMessageTable();
   resolveMetaAttributeTable();

   env->Table->gc_rootcount = (_linker->getStaticCount() << 2);
}

ident_t Instance :: getSubject(ref_t subjectRef)
{
   return _linker->retrieveResolvedAction(subjectRef);
}

lvaddr_t Instance :: loadSymbol(ident_t reference, int mask, bool silentMode)
{
   // reference should not be a forward one
   while (isForwardReference(reference)) {
      ident_t resolved = resolveForward(reference + FORWARD_PREFIX_NS_LEN);
      if (emptystr(resolved)) {

         throw JITUnresolvedException(reference);
      }
      else reference = resolved;
   }
   return _linker->resolve(reference, mask, silentMode);
}

int Instance :: loadMessageName(mssg_t message, char* buffer, size_t maxLength)
{
   int prefixLen = 0;
   ref_t action, flags;
   pos_t count;
   decodeMessage(message, action, count, flags);
   if ((flags & PREFIX_MESSAGE_MASK) == VARIADIC_MESSAGE) {
      size_t len = 7;
      Convertor::copy(buffer, "params#", 7, len);

      prefixLen += len;
      buffer += len;
      maxLength -= len;
   }

   int used = 0;
   ident_t subjectName = getSubject(action);
   size_t length = getlength(subjectName);
   if (length > 0) {
      if (maxLength >= (int)(length + used)) {
         Convertor::copy(buffer + used, subjectName, length, length);

         used += length;
      }
      else buffer[used] = 0;
   }

   if (count > 0) {
      size_t dummy = 10;
      String<char, 10>temp;
      temp.appendInt(count);

      buffer[used++] = '[';
      Convertor::copy(buffer + used, temp, getlength(temp), dummy);
      used += dummy;
      buffer[used++] = ']';
   }
   buffer[used] = 0;

   return used + prefixLen;
}

ref_t Instance :: loadDispatcherOverloadlist(ident_t referenceName)
{
   return (ref_t)loadMetaAttribute(referenceName, caExtOverloadlist);
}

void* Instance :: loadMetaAttribute(ident_t name, int category)
{
   MemoryReader reader(_mattributeTable);

   size_t len = /*reader.getDWord()*/_ConvertedMATSize;

   RTManager manager;

   return (void*)manager.loadMetaAttribute(reader, name, category, len);
}

int Instance :: loadExtensionDispatcher(SystemEnv* env, const char* moduleList, mssg_t message, void* output)
{
   // load message name
   char messageName[IDENTIFIER_LEN];
   int mssgLen = loadMessageName(message, messageName, IDENTIFIER_LEN);
   messageName[mssgLen] = 0;

   int len = 0;

   // search message dispatcher
   IdentifierString messageRef;
   int listLen = getlength(moduleList);
   int i = 0;
   while (moduleList[i]) {
      ident_t ns = moduleList + i;

      // HOT-FIX : load a module attributes in VM
      messageRef.copy(ns);
      messageRef.append('\'');
      messageRef.append(NAMESPACE_REF);
      LoadResult result;
      ref_t dummy = 0;
      resolveModule(messageRef, result, dummy);
      if (_linker->withNewInitializers())
         onNewInitializers(env);

      messageRef.copy(ns);
      messageRef.append('\'');
      messageRef.append(messageName);

      ref_t listRef = loadDispatcherOverloadlist(messageRef.c_str());
      if (listRef) {
         ((int*)output)[len] = listRef;
         len++;
      }

      i += getlength(ns) + 1;
   }

   return len;
}

bool Instance :: initLoader(InstanceConfig& config)
{
   // load paths
   _loader.setRootPath(config.libPath.c_str());

   // load primitives
   Primitives::Iterator it = config.primitives.start();
   while (!it.Eof()) {
      Path path(*it);
      if (it.key().compare(CORE_ALIAS)) {
         _loader.addCorePath(path.c_str());
      }
      else _loader.addPrimitivePath(it.key(), path.c_str());

      it++;
   }

   return true;
}

void Instance :: clearMessageTable()
{
   _messageTable->trim(0);
   _messageBodyTable->trim(0);

   _messageTable->writeBytes(0, 0, sizeof(uintptr_t) * 2); // write dummy place holder
   _messageBodyTable->writeBytes(0, 0, sizeof(uintptr_t)); // write dummy place holder
}

void Instance :: clearMetaAttributeTable()
{
   _mattributeTable->trim(0);
}

void Instance :: resolveMessageTable()
{
   while (_messageTable->Length() > _ConvertedMTSize) {
      // !! HOTFIX : the message section should be overwritten
      getMessageSection()->trim(0);

      _ConvertedMTSize = _messageTable->Length();

      _linker->resolve(MESSAGE_TABLE, mskMessageTableRef, true);
   }
}

void Instance :: resolveMetaAttributeTable()
{
   while (_mattributeTable->Length() > _ConvertedMATSize) {
      // !! HOTFIX : the message section should be overwritten
      getMetaAttributeSection()->trim(0);

      _ConvertedMATSize = _mattributeTable->Length();

      _linker->resolve(MATTRIBUTE_TABLE, mskMetaAttributes, true);
   }
}

bool Instance :: restart(SystemEnv* env, void* sehTable, bool debugMode, bool withExtDispatchers)
{
   printInfo(ELENAVM_GREETING, ENGINE_MAJOR_VERSION, ENGINE_MINOR_VERSION, ELENAVM_REVISION);
   printInfo(ELENAVM_INITIALIZING);

   _withExtDispatchers = withExtDispatchers;

   clearReferences();
   clearMessageTable();
   _ConvertedMTSize = 0;

   //TODO: clear message table?

   _superClass.copy(_config.forwards.get(SUPER_FORWARD));
   _literalClass.copy(_config.forwards.get(STR_FORWARD));
   _wideLiteralClass.copy(_config.forwards.get(WIDESTR_FORWARD));
   _characterClass.copy(_config.forwards.get(CHAR_FORWARD));
   _intClass.copy(_config.forwards.get(INT_FORWARD));
   _realClass.copy(_config.forwards.get(REAL_FORWARD));
   _longClass.copy(_config.forwards.get(LONG_FORWARD));
   _msgClass.copy(_config.forwards.get(MESSAGE_FORWARD));
   //_extMsgClass.copy(_config.forwards.get(EXT_MESSAGE_FORWARD));
   _subjClass.copy(_config.forwards.get(MESSAGENAME_FORWARD));

   // init debug section
   if (_debugMode) {
      printInfo(ELENAVM_DEBUGINFO);
   }

   _compiler->setTLSKey(*env->TLSIndex);
   _compiler->setThreadTable((lvaddr_t)env->ThreadTable);
   _compiler->setEHTable((lvaddr_t)sehTable);
   _compiler->setGCTable((lvaddr_t)env->Table);

   // load predefined code
   _linker->prepareCompiler();

   // HOTFIX : literal constant is refered in the object, so it should be preloaded
   _linker->resolve(_literalClass, mskVMTRef, true);

   _linker->fixImage(_superClass.ident());

   // HOTFIX : resolve message table
   resolveMessageTable();

   _initialized = true;

   // set debug ptr if requiered
   if (_debugMode) {
      env->Table->dbg_ptr = (uintptr_t)loadDebugSection();
   }

   // HOTFIX : set gc_roots
   env->Table->gc_roots = (uintptr_t)getTargetSection(mskStatRef)->get(0);
   env->Table->gc_rootcount = (_linker->getStaticCount() << 2);

   printInfo(ELENAVM_DONEINFO);

   return true;
}

void Instance :: translate(MemoryReader& reader, ImageReferenceHelper& helper, MemoryDump& dump, int terminator)
{
   MemoryWriter ecodes(&dump);

   ecodes.writeDWord(0);            // write size place holder
   pos_t procPtr = ecodes.Position();

   // open 0
   ecodes.writeByte(bcOpen);

   // resolve tape
   size_t command = reader.getDWord();
   lvaddr_t  extra_param = 0;
   while (command != terminator) {
      ident_t arg = NULL;
      size_t param = reader.getDWord();
      if (test(command, LITERAL_ARG_MASK)) {
         arg = (const char*)reader.Address();

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
         //case PUSH_VAR_MESSAGE_ID:
         //   // pushfi param
         //   ecodes.writeByte(bcPushFI);
         //   ecodes.writeDWord(param);
         //   break;
         //case ASSIGN_VAR_MESSAGE_ID:
         //   // popa
         //   // asavefi param
         //   ecodes.writeByte(bcPopA);
         //   ecodes.writeByte(bcASaveFI);
         //   ecodes.writeDWord(param);
         //   break;
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
         case PUSHE_TAPE_MESSAGE_ID:
            // pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol(arg, mskExtMessage), mskRDataRef);
            break;
         case PUSHG_TAPE_MESSAGE_ID:
            // pushr r
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, loadSymbol(arg, mskMessageName), mskRDataRef);
            break;
         case POP_TAPE_MESSAGE_ID:
            //freei param
            ecodes.writeByte(bcFreeI);
            ecodes.writeDWord(param);
            break;
         case SEND_TAPE_MESSAGE_ID:
            //copym message
            //aloadsi 0
            //acallvi 0
            //pusha

            ecodes.writeByte(bcMovM);
            ecodes.writeDWord(_linker->parseMessage(arg));
            ecodes.writeByte(bcPeekSI);
            ecodes.writeDWord(0);
            ecodes.writeByte(bcCallVI);
            ecodes.writeDWord(0);
            ecodes.writeByte(bcPushA);

            break;
         case NEW_TAPE_MESSAGE_ID:
         {
            ecodes.writeByte(bcPushR);
            helper.writeTape(ecodes, extra_param, /*mskVMTRef*/mskRDataRef);

            IdentifierString message;
            message.append('0' + (char)(param + 1));
            message.append(CONSTRUCTOR_MESSAGE);

            ecodes.writeByte(bcMovM);
            ecodes.writeDWord(_linker->parseMessage(message.c_str()));
            ecodes.writeByte(bcPeekSI);
            ecodes.writeDWord(0);
            ecodes.writeByte(bcCallVI);
            ecodes.writeDWord(0);
            ecodes.writeByte(bcPushA);
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

bool Instance :: loadTemplate(ident_t name)
{
   Path path(_machine->templates.get(name));

   if (!_config.load(path.c_str(), &_machine->templates)) {
      setStatus("Cannot load the template:", name);

      return false;
   }

   return initLoader(_config);
}

void Instance :: setPackagePath(ident_t package, path_t path)
{
   _loader.setNamespace(package, path);
}

void Instance :: setPackagePath(ident_t line)
{
   size_t sep = line.find('=', -1);
   if (sep != -1) {
      Path path(line + sep + 1);
      IdentifierString package(line, sep);

      setPackagePath(package, path.c_str());
   }
   else {
      Path path(line);

      setPackagePath(NULL, path.c_str());
   }
}

void Instance :: addPackagePath(ident_t package, ident_t path)
{
   _loader.addPackage(package, path);
}

void Instance :: addPackagePath(ident_t line)
{
   size_t sep = line.find('=', -1);
   if(sep != -1) {
      IdentifierString path(line + sep + 1);
      IdentifierString package(line, sep);

      addPackagePath(package, path);
   }
   else addPackagePath(NULL, line);
}

void Instance :: configurate(SystemEnv* env, void* sehTable, MemoryReader& reader, int terminator)
{
   size_t pos = reader.Position();

   size_t command = reader.getDWord();
   while (command != terminator) {
      ident_t arg = NULL;
      size_t param = reader.getDWord();
      if (test(command, LITERAL_ARG_MASK)) {
         arg = (const char*)reader.Address();

         reader.seek(reader.Position() + param);  // goes to the next record
      }

      switch (command) {
         case EXT_DISPATCHER_ON:
            _withExtDispatchers = true;
            break;
         case USE_VM_MESSAGE_ID:
            if (emptystr(_loader.getNamespace())) {
               setPackagePath(arg);
            }
            else addPackagePath(arg);
            break;
         case MAP_VM_MESSAGE_ID:
            addForward(arg);
            break;
         case LOAD_VM_MESSAGE_ID:
            if(!loadTemplate(arg))
               throw EAbortException();

            break;
         case OPEN_VM_CONSOLE:
            if (_debugMode)
               createConsole();
            break;
         case START_VM_MESSAGE_ID:
            createConsole();

            if(!restart(env, sehTable, _debugMode, _withExtDispatchers))
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

void Instance :: onNewInitializers(SystemEnv* env)
{
   stopVM();

   // create byte code tape
   MemoryDump           ecodes;
   ImageReferenceHelper helper(this);

   ecodes.writeDWord(0, 0);

   // generate module initializers
   MemoryDump  initTape(0);
   _linker->generateInitTape(initTape);
   if (initTape.Length() > 0) {
      ecodes[0] = ecodes[0] + initTape.Length();
      ecodes.insert(4, initTape.get(0), initTape.Length());
   }

   // compile byte code
   MemoryReader reader(&ecodes);

   lvaddr_t vaddress = _linker->resolveTemporalByteCode(helper, reader, TAPE_SYMBOL, nullptr);

   // update debug section size if available
   if (_debugMode) {
      _Memory* debugSection = getTargetDebugSection();

      (*debugSection)[0] = debugSection->Length();

      //// add subject list to the debug section
      //_ELENA_::MemoryWriter debugWriter(debugSection);
      //saveActionNames(&debugWriter);
   }

   onNewCode(env);

   resumeVM();
}

int Instance :: interprete(SystemEnv* env, void* sehTable, void* tape, bool standAlone)
{
   ByteArray    tapeArray(tape, -1);
   MemoryReader tapeReader(&tapeArray);

   stopVM();

   // configurate VM instance
   configurate(env, sehTable, tapeReader, 0);

   if (!_initialized)
      throw InternalError("ELENAVM is not initialized");

   // exit if nothing to interprete
   if (tapeArray[tapeReader.Position()] == 0)
      return -1;

   //if (_debugMode) {
   //   // remove subject list from the debug section
   //   _Memory* debugSection = getTargetDebugSection();
   //   if ((*debugSection)[0] > 0)
   //      debugSection->trim((*debugSection)[0]);
   //}

   // !! probably, it is better to use jitlinker reference helper class
   ImageReferenceHelper helper(this);

   // create byte code tape
   MemoryDump   ecodes;
   translate(tapeReader, helper, ecodes, 0);

   // generate module initializers
   MemoryDump  initTape(0);
   _linker->generateInitTape(initTape);
   if (initTape.Length() > 0) {
      ecodes[0] = ecodes[0] + initTape.Length();
      ecodes.insert(4, initTape.get(0), initTape.Length());
   }

   // compile byte code
   MemoryReader reader(&ecodes);

   lvaddr_t vaddress = _linker->resolveTemporalByteCode(helper, reader, TAPE_SYMBOL, tape);

   // update debug section size if available
   if (_debugMode) {
      _Memory* debugSection = getTargetDebugSection();

      (*debugSection)[0] = debugSection->Length();

      //// add subject list to the debug section
      //_ELENA_::MemoryWriter debugWriter(debugSection);
      //saveActionNames(&debugWriter);
   }

   onNewCode(env);

   resumeVM();

   // raise an exception to warn debugger
   if (_debugMode) {
      raiseBreakpoint();
   }

   _Entry entry;
   entry.address = env->Invoker;

   int retVal = 0;
   if (!standAlone) {
      // HOTFIX : load invoker
      entry.address = (void*)_compiler->getInvoker();

      retVal = __routineProvider.ExecuteInFrame(env, entry, (void*)vaddress);
   }
   else retVal = entry.evaluate2(0, (void*)vaddress);

   if (retVal == 0)
      setStatus("Broken");

   return retVal;
}

bool Instance :: loadAddressInfo(void* address, char* buffer, size_t& maxLength)
{
   RTManager manager;
   MemoryReader reader(getTargetDebugSection(), 8u);
   reader.getLiteral(DEFAULT_STR);

   maxLength = manager.readAddressInfo(reader, (size_t)address, &_loader, buffer, maxLength);

   return maxLength > 0;
}

void* Instance :: parseMessage(SystemEnv* systemEnv, ident_t message)
{
   IdentifierString messageName;
   pos_t paramCount = -1;
   ref_t flags = 0;

   if (SystemRoutineProvider::parseMessageLiteral(message, messageName, paramCount, flags)) {
      ref_t actionRef = getSubjectRef(systemEnv, messageName.ident());
      if (!actionRef)
         return nullptr;

      return (void*)(encodeMessage(actionRef, paramCount, flags));
   }
   else return nullptr;
}

// --- ELENAMachine::Config ---

bool ELENAVMMachine::Config :: load(path_t path, Templates* templates)
{
   XmlConfigFile config;
   _ELENA_::Path rootPath;

   rootPath.copySubPath(path);

   if (_ELENA_::emptystr(path) || !config.load(path, feUTF8)) {
      return false;
   }

   // load templates
   if (templates) {
      loadList(config, TEMPLATE_CATEGORY, rootPath.c_str(), templates);
   }

   init(rootPath.c_str(), config);

   return true;
}

// --- ELENAVMMachine ---

ELENAVMMachine :: ELENAVMMachine(path_t rootPath)
   : templates(NULL, freestr), _rootPath(rootPath)
{
   Path configPath(rootPath, CONFIG_PATH);

   config.load(configPath.c_str(), &templates);
}

void ELENAVMMachine :: startSTA(ProgramHeader* frameHeader, SystemEnv* env, void* sehTable, void* tape)
{
   // setting up system
   __routineProvider.InitSTA((SystemEnv*)env, frameHeader);

   if (tape != nullptr) {
      int retVal = 0;
      try {
         // if it is a stand alone application
         _instance->interprete(env, sehTable, tape, true);
      }
      catch (JITUnresolvedException& e)
      {
         retVal = -1;
         _instance->setStatus("Cannot load ", e.referenceInfo);

         _instance->printInfo(_instance->getStatus());
      }
      catch(InternalError& e)
      {
         _instance->printInfo("InternalError");

         retVal = -1;
         _instance->setStatus(e.message);
      }
      catch (EAbortException&)
      {
         _instance->printInfo("EAbortException");

         retVal = -1;
      }

      // winding down system
      Exit(retVal);
   }
   // if it is part of the terminal - do nothing
}

void ELENAVMMachine :: Exit(int exitCode)
{
   __routineProvider.Exit(exitCode);
}
