//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Machine declaration
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "elenartmachine.h"
#include "bytecode.h"
#include "rtmanager.h"
#include "langcommon.h"

using namespace elena_lang;

// --- ELENARTMachine ---

ELENARTMachine :: ELENARTMachine(path_t dllRootPath, path_t execPath, path_t configFile, PlatformType platform, void* mdata)
   : _execPath(execPath), _platform(platform), _mdata(mdata)
{
   _debugFilePath.copy(execPath);
   _debugFilePath.changeExtension("dn");

   PathString configPath(dllRootPath, configFile);
   loadConfig(*configPath);

   _providerInitialized = false;
}

void ELENARTMachine :: loadConfig(ConfigFile& config, path_t configPath, ConfigFile::Node root)
{
   auto configNode = config.selectNode(root, LIB_PATH);
   if (!configNode.isNotFound()) {
      DynamicString<char> value;
      configNode.readContent(value);

      PathString path(configPath, value.str());

      _libraryProvider.setRootPath(*path);
   }
}

ConfigFile::Node getPlatformRoot(ConfigFile& config, PlatformType platform)
{
   ustr_t key = getPlatformName(platform);

   // select platform configuration
   ConfigFile::Node platformRoot = config.selectNode<ustr_t>(PLATFORM_CATEGORY, key, [](ustr_t key, ConfigFile::Node& node)
      {
         return node.compareAttribute("key", key);
      });

   return platformRoot;
}

void ELENARTMachine :: loadConfig(path_t path)
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
   }
}

bool ELENARTMachine :: loadDebugSection()
{
   if (_debugFilePath.empty())
      return false;

   FileReader reader(*_debugFilePath, FileRBMode, FileEncoding::Raw, false);
   if (!reader.isOpen()) {
      // clear the path to indicate an absence of debug data
      _debugFilePath.clear();

      return false;
   }
   else {
      char header[8];
      reader.read(header, 8);
      if (!ustr_t(DEBUG_MODULE_SIGNATURE).compare(header, 5)) {
         // clear the path to indicate invalid debug data
         _debugFilePath.clear();

         return false;
      }

      pos_t len = reader.getDWord();
      MemoryWriter writer(&_debugSection);
      writer.copyFrom(&reader, len);

      return len != 0;
   }
}

addr_t ELENARTMachine :: retrieveGlobalAttribute(int attribute, ustr_t name)
{
   ImageSection msection(_mdata, 0x1000000);
   RTManager rtmanager(&msection, nullptr);

   return rtmanager.retrieveGlobalAttribute(attribute, name);
}

size_t ELENARTMachine :: loadClassName(addr_t classAddress, char* buffer, size_t length)
{
   return RTManager::loadClassName(classAddress, buffer, length);
}

addr_t ELENARTMachine :: loadSymbol(ustr_t name)
{
   return retrieveGlobalAttribute(GA_SYMBOL_NAME, name);
}

addr_t ELENARTMachine :: loadClassReference(ustr_t name)
{
   return retrieveGlobalAttribute(GA_CLASS_NAME, name);
}

void ELENARTMachine :: loadSubjectName(IdentifierString& actionName, ref_t subjectRef)
{
   ImageSection msection(_mdata, 0x1000000);
   RTManager rtmanager(&msection, nullptr);

   rtmanager.loadSubjectName(actionName, subjectRef);
}

int ELENARTMachine :: loadSignature(mssg_t message, addr_t* output, pos_t maximalCount)
{
   ImageSection msection(_mdata, 0x1000000);
   RTManager rtmanager(&msection, nullptr);

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

size_t ELENARTMachine :: loadMessageName(mssg_t message, char* buffer, size_t length)
{
   ref_t actionRef, flags;
   pos_t argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);

   IdentifierString actionName;
   loadSubjectName(actionName, actionRef);

   IdentifierString messageName;
   ByteCodeUtil::formatMessageName(messageName, nullptr, *actionName, nullptr, 0, argCount, flags);

   if ((message & PREFIX_MESSAGE_MASK) == CONVERSION_MESSAGE) {
      size_t position = (*messageName).find('[');

      ImageSection msection(_mdata, 0x1000000);
      RTManager rtmanager(&msection, nullptr);

      addr_t signatures[ARG_COUNT] = {};
      if(rtmanager.loadSignature(actionRef, argCount, signatures)) {
         for (pos_t i = 0; i < argCount; i++) {
            char tmp[IDENTIFIER_LEN];

            size_t len = loadClassName(signatures[i], tmp, IDENTIFIER_LEN);
            tmp[len] = 0;

            if (i == 0) {
               messageName.insert("<>", position);
               position++;
            }
            else {
               messageName.insert(",", position);
               position++;
            }

            messageName.insert(tmp, position);
            position += (len + 1);
         }
      }
   }

   StrConvertor::copy(buffer, *messageName, messageName.length(), length);

//   buffer[length] = 0;
//   printf("loadMessageName %s\n", buffer);

   return length;
}

size_t ELENARTMachine :: loadActionName(mssg_t message, char* buffer, size_t length)
{
   ref_t actionRef, flags;
   pos_t argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);

   IdentifierString actionName;
   loadSubjectName(actionName, actionRef);

   StrConvertor::copy(buffer, *actionName, actionName.length(), length);

   return length;
}

ref_t ELENARTMachine :: loadSubject(ustr_t actionName)
{
   ImageSection msection(_mdata, 0x1000000);
   RTManager rtmanager(&msection, nullptr);

   return rtmanager.loadSubject(actionName);
}

mssg_t ELENARTMachine :: loadMessage(ustr_t messageName)
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

mssg_t ELENARTMachine :: loadAction(ustr_t actionName)
{
   pos_t argCount = 0;
   ref_t flags = 0;

   ref_t actionRef = loadSubject(actionName);
   if (!actionRef)
      return 0;

   return encodeMessage(actionRef, argCount, flags);
}

addr_t ELENARTMachine :: loadDispatcherOverloadlist(ustr_t referenceName)
{
   return retrieveGlobalAttribute(GA_EXT_OVERLOAD_LIST, referenceName);
}

int ELENARTMachine :: loadExtensionDispatcher(const char* moduleList, mssg_t message, void* output)
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

   // HOTFIX : putting terminator
   ((addr_t*)output)[len] = 0;

   return len;
}

size_t ELENARTMachine :: loadAddressInfo(addr_t retPoint, char* lineInfo, size_t length)
{
   // lazy load of debug data
   if (_debugSection.length() == 0 && !loadDebugSection())
      return 0;

   RTManager rtmanager(nullptr, &_debugSection);
   if (!_providerInitialized) {
      PathString rootPath;
      rootPath.copySubPath(*_execPath, true);

      rtmanager.loadRootPackage(_libraryProvider, *rootPath);

      _providerInitialized = true;
   }

   return rtmanager.retriveAddressInfo(_libraryProvider, retPoint, lineInfo, length, false);
}

void ELENARTMachine :: Exit(int exitCode)
{
   __routineProvider.Exit(exitCode);
}

void ELENARTMachine :: startSTA(SystemEnv* env, void* entry)
{
   // setting up system
   __routineProvider.InitSTA(env);

   // executing the program
   execute(env, entry);

   // winding down system
   Exit(0);
}

size_t ELENARTMachine :: allocateThreadEntry(SystemEnv* env)
{
   if (env->th_table && env->th_table->counter < env->threadCounter) {
      size_t index = env->th_table->counter;

      env->th_table->counter++;

      return index;
   }

   return INVALID_SIZE;
}

void ELENARTMachine :: clearThreadEntry(SystemEnv* env, size_t index)
{
   env->th_table->slots[index].content = nullptr;
   env->th_table->slots[index].arg = nullptr;
}

void* ELENARTMachine :: allocateThread(SystemEnv* env, void* arg, void* threadProc, int stackSize, int flags)
{
   size_t index = allocateThreadEntry(env);

   if (index == INVALID_SIZE)
      return nullptr;

   env->th_table->slots[index].arg = arg;

   return __routineProvider.CreateThread(index, stackSize, flags, threadProc);
}

void ELENARTMachine :: startThread(SystemEnv* env, void* entry, int index)
{
   void* arg = env->th_table->slots[index].arg;
   // executing the program
   executeDirectly(entry, arg);
}

bool ELENARTMachine :: checkClassMessage(void* classPtr, mssg_t message)
{
   ImageSection msection(_mdata, 0x1000000);

   return SystemRoutineProvider::CheckMessage(&msection, classPtr, message);
}

size_t ELENARTMachine :: loadClassMessages(void* classPtr, mssg_t* output, size_t skip, size_t maxLength)
{
   ImageSection msection(_mdata, 0x1000000);

   return SystemRoutineProvider::LoadMessages(&msection, classPtr, output, skip, 
      maxLength, false);
}
