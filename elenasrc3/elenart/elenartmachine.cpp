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

addr_t ELENARTMachine :: loadSymbol(ustr_t name)
{
   return 0;
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

size_t ELENARTMachine :: loadAddressInfo(addr_t retPoint, char* lineInfo, size_t length)
{
   // lazy load of debug data
   if (_debugSection.length() == 0 && !loadDebugSection())
      return 0;

   RTManager rtmanager(nullptr, &_debugSection);

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

   // winding down system
   Exit(retVal);
}
