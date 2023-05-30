//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA RT Machine declaration
//
//                                             (C)2021-2023, by Aleksey Rakov
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

size_t ELENARTMachine :: loadMessageName(mssg_t message, char* buffer, size_t length)
{
   ref_t actionRef, flags;
   pos_t argCount = 0;
   decodeMessage(message, actionRef, argCount, flags);

   IdentifierString actionName;
   loadSubjectName(actionName, actionRef);

   IdentifierString messageName;
   ByteCodeUtil::formatMessageName(messageName, nullptr, *actionName, nullptr, 0, argCount, flags);

   StrConvertor::copy(buffer, *messageName, messageName.length(), length);

//   buffer[length] = 0;
//   printf("loadMessageName %s\n", buffer);

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

   return rtmanager.retriveAddressInfo(_libraryProvider, retPoint, lineInfo, length);
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
   int retVal = execute(env, entry);

   printf("exiting");

   unsigned n = 0x800000;
   unsigned m = 0x800000;
   bool r = n < m;

   // winding down system
   Exit(retVal);
}

int ELENARTMachine :: allocateThreadEntry(SystemEnv* env)
{
   if (env->th_table->counter < env->threadCounter) {
      int index = env->th_table->counter;

      env->th_table->counter++;

      return index;
   }

   return -1;
}

void* ELENARTMachine :: allocateThread(SystemEnv* env, void* arg, void* threadProc, int flags)
{
   int index = allocateThreadEntry(env);
   if (index == -1)
      return nullptr;

   env->th_table->slots[index].arg = arg;

   return __routineProvider.CreateThread(index, flags, threadProc);
}

void ELENARTMachine :: startThread(SystemEnv* env, void* entry, int index)
{
   void* arg = env->th_table->slots[index].arg;
   // executing the program
   int retVal = execute(env, entry, arg);

   // winding down thread
   //ExitThread(retVal);
}
