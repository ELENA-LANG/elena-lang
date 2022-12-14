//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the project class body
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "elena.h"
// --------------------------------------------------------------------------
#include "project.h"
#include "config.h"
#include "cliconst.h"

using namespace elena_lang;

PlatformType operator & (const PlatformType& l, const PlatformType& r)
{
   return (PlatformType)((int)l & (int)r);
}

PlatformType operator | (const PlatformType& l, const PlatformType& r)
{
   return (PlatformType)((int)l | (int)r);
}

// --- Project ---

path_t Project :: PathSetting(ProjectOption option, ustr_t key) const
{
   ProjectNode current = _root.findChild(option).firstChild();
   while (current != ProjectOption::None) {
      ProjectNode keyNode = current.findChild(ProjectOption::Key);
      if (keyNode != ProjectOption::None && key.compare(keyNode.identifier())) {
         return _paths.get(current.arg.value);
      }

      current = current.nextNode();
   }

   return nullptr;
}

path_t Project :: PathSetting(ProjectOption option) const
{
   int fileIndex = 0;
   switch (option) {
      case ProjectOption::BasePath:
         return *_basePath;
      case ProjectOption::ProjectPath:
         return *_projectPath;
      case ProjectOption::TargetPath:
      case ProjectOption::OutputPath:
      case ProjectOption::LibPath:
         fileIndex = _root.findChild(option).arg.value;
         break;
      default:
         return nullptr;
   }

   return _paths.get(fileIndex);
}

ustr_t Project :: StringSetting(ProjectOption option) const
{
   switch (option) {
      case ProjectOption::Namespace:
         return *_defaultNs;
      default:
      {
         ProjectNode node = _root.findChild(option);

         return node.identifier();
      }
   }
}

bool Project :: BoolSetting(ProjectOption option, bool defValue) const
{
   ustr_t val = StringSetting(option);
   if (val.empty())
      return defValue;

   return val == "-1";
}

void Project :: addBoolSetting(ProjectOption option, bool value)
{
   _root.appendChild(option, value ? "-1" : "0");
}

void Project :: addIntSetting(ProjectOption option, int value)
{
   String<char, 20> valStr;
   valStr.appendInt(value);

   _root.appendChild(option, valStr.str());
}

int Project :: IntSetting(ProjectOption option, int defValue) const
{
   ustr_t val = StringSetting(option);
   if (!val.empty()) {
      int intValue = StrConvertor::toInt(val, 10);

      return intValue;
   }
   else return defValue;
}

ustr_t Project :: resolveKey(ProjectOption category, ProjectOption item, ustr_t key)
{
   ProjectNode current = ProjectTree::gotoChild(_root.findChild(category), item, key);

   return current == item ? current.findChild(ProjectOption::Value).identifier() : nullptr;
}

PlatformType Project :: SystemTarget()
{
   return _platform & PlatformType::TargetMask;
}

PlatformType Project :: Platform()
{
   return _platform & PlatformType::PlatformMask;
}

PlatformType Project::TargetType()
{
   return _platform & PlatformType::TargetTypeMask;
}

ustr_t Project :: resolveForward(ustr_t forward)
{
   return _forwards.get(forward);
}

ustr_t Project::resolveExternal(ustr_t dllAlias)
{
   return resolveKey(ProjectOption::Externals, ProjectOption::External, dllAlias);
}

ustr_t Project :: resolveWinApi(ustr_t dllAlias)
{
   return resolveKey(ProjectOption::Winapis, ProjectOption::Winapi, dllAlias);
}

void Project :: addForward(ustr_t forward, ustr_t referenceName)
{
   freeUStr(_forwards.exclude(forward));

   _forwards.add(forward, referenceName.clone());
}

void Project :: addSource(ustr_t ns, path_t path)
{
   if (!_loaded && _projectName.empty())
      _projectName.copy(ns);

   ProjectNode files = _root.findChild(ProjectOption::Files);
   if (files == ProjectOption::None) {
      files = _root.appendChild(ProjectOption::Files);
   }

   ProjectNode current = files.firstChild();
   while (current != ProjectOption::None && !ns.compare(current.identifier())) {
      current = current.nextNode();
   }
   if (current == ProjectOption::None) {
      current = files.appendChild(ProjectOption::Module, ns);
   }

   current.appendChild(ProjectOption::FileKey, _paths.count() + 1);

   PathString fullPath(*_projectPath, path);
   _paths.add((*fullPath).clone());
}

void Project :: addPathSetting(ProjectOption key, path_t path)
{
   ProjectNode node = _root.findChild(key);
   if (node == ProjectOption::None) {
      node = _root.appendChild(key, _paths.count() + 1);
   }
   else node.setArgumentValue(_paths.count() + 1);

   _paths.add(path.clone());
}

void Project :: addStringSetting(ProjectOption key, ustr_t s)
{
   ProjectNode node = _root.findChild(key);
   if (node == ProjectOption::None) {
      _root.appendChild(key, s);
   }
   else node.setStrArgument(s);
}

void Project :: loadSourceFiles(ConfigFile& config, ConfigFile::Node& configRoot)
{
   DynamicString<char> subNs;
   DynamicString<char> path;

   ConfigFile::Collection modules;
   if (config.select(configRoot, MODULE_CATEGORY, modules)) {
      for (auto m_it = modules.start(); !m_it.eof(); ++m_it) {
         ConfigFile::Node moduleNode = *m_it;

         if (!moduleNode.readAttribute("name", subNs)) {
            subNs.clear();
         }

         ReferenceName          ns(Namespace(), subNs.str());
         ConfigFile::Collection files;
         if (config.select(moduleNode, "*", files)) {
            for (auto it = files.start(); !it.eof(); ++it) {
               // add source file
               ConfigFile::Node node = *it;
               node.readContent(path);

               PathString filePath(path.str());
               addSource(*ns, *filePath);
            }
         }
      }
   }
}

void Project :: loadPathSetting(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath, 
                                ProjectOption key, path_t configPath)
{
   auto configNode = config.selectNode(configRoot, xpath);
   if (!configNode.isNotFound()) {
      DynamicString<char> path;
      configNode.readContent(path);

      ProjectNode node = _root.findChild(key);
      if (node == ProjectOption::None) {
         _root.appendChild(key, _paths.count() + 1);
      }
      else node.setArgumentValue(_paths.count() + 1);

      PathString filePath(configPath, path.str());
      _paths.add((*filePath).clone());
   }
}

void Project :: loadSetting(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath, IdentifierString& value)
{
   auto configNode = config.selectNode(configRoot, xpath);
   if (!configNode.isNotFound()) {
      DynamicString<char> content;
      configNode.readContent(content);

      value.copy(content.str());
   }
}

void Project::loadTargetType(ConfigFile& config, ConfigFile::Node& root)
{
   // read target type; merge it with platform if required
   ConfigFile::Node targetType = config.selectNode(root, PLATFORMTYPE_KEY);
   if (!targetType.isNotFound()) {
      DynamicString<char> key;
      targetType.readContent(key);

      PlatformType pt = (PlatformType)key.toInt();
      _platform = _platform & PlatformType::PlatformMask;
      _platform = _platform | pt;
   }
}

void Project :: loadPathCollection(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath,
                                   ProjectOption collectionKey, path_t configPath)
{
   DynamicString<char> path;

   ConfigFile::Collection collection;
   if(config.select(configRoot, xpath, collection)) {
      auto collectionNode = _root.findChild(collectionKey);
      if (collectionNode == ProjectOption::None)
         collectionNode = _root.appendChild(collectionKey);

      for (auto it = collection.start(); !it.eof(); ++it) {
         ConfigFile::Node node = *it;

         ProjectNode fileNode = collectionNode.appendChild(ProjectOption::FileKey, _paths.count() + 1);

         // read the key
         if (node.readAttribute("key", path)) {
            fileNode.appendChild(ProjectOption::Key, path.str());
         }

         // read the path
         node.readContent(path);
         PathString filePath(configPath, path.str());
         _paths.add((*filePath).clone());
      }
   }
}

void Project::loadKeyCollection(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath, ProjectOption collectionKey,
   ProjectOption itemKey, ustr_t prefix)
{
   DynamicString<char> key, value;

   ConfigFile::Collection collection;
   if (config.select(root, xpath, collection)) {
      auto collectionNode = _root.findChild(collectionKey);
      if (collectionNode == ProjectOption::None)
         collectionNode = _root.appendChild(collectionKey);

      for (auto it = collection.start(); !it.eof(); ++it) {
         ConfigFile::Node node = *it;

         if (node.readAttribute("key", key)) {
            node.readContent(value);

            ProjectNode keyNode = ProjectTree::gotoChild(collectionNode, itemKey, key.str());
            if (keyNode == ProjectOption::None) {
               if (!prefix.empty())
                  key.insert(prefix, 0);

               keyNode = collectionNode.appendChild(itemKey, key.str());
            }               

            ProjectNode valueNode = keyNode.findChild(ProjectOption::Value);
            if (valueNode == ProjectOption::None) {
               keyNode.appendChild(ProjectOption::Value, value.str());
            }
            else keyNode.setStrArgument(value.str());
         }
      }
   }
}

void Project :: loadForwards(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath)
{
   DynamicString<char> key, value;

   ConfigFile::Collection collection;
   if (config.select(root, xpath, collection)) {
      for (auto it = collection.start(); !it.eof(); ++it) {
         ConfigFile::Node node = *it;

         if (node.readAttribute("key", key)) {
            node.readContent(value);

            addForward(key.str(), value.str());
         }
      }
   }
}

void Project :: copySetting(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath, ProjectOption key, bool exclusiveMode)
{
   auto configNode = config.selectNode(configRoot, xpath);
   if (!configNode.isNotFound()) {
      DynamicString<char> content;
      configNode.readContent(content);

      if (!exclusiveMode || !_root.existChild(key)) {
         _root.appendChild(key, content.str());
      }
      else {
         auto node = _root.findChild(key);

         node.setStrArgument(content.str());
      }
   }
}

bool Project :: loadConfigByName(path_t configPath, ustr_t name, bool markAsLoaded)
{
   path_t relativePath = PathSetting(ProjectOption::Templates, name);
   if (!relativePath.empty()) {
      PathString baseConfigPath(configPath, relativePath);

      if (loadConfig(*baseConfigPath, false)) {
         if (markAsLoaded)
            _loaded = true;

         return true;
      }
   }
   return false;
}

void Project :: loadConfig(ConfigFile& config, path_t configPath, ConfigFile::Node& root)
{
   if (!root.isNotFound()) {
      loadPathCollection(config, root, TEMPLATE_CATEGORY, ProjectOption::Templates, configPath);

      ConfigFile::Node baseConfig = config.selectNode(root, PROJECT_TEMPLATE);
      if (!baseConfig.isNotFound()) {
         DynamicString<char> key;
         baseConfig.readContent(key);

         loadConfigByName(configPath, key.str(), false);
      }

      loadSetting(config, root, NAMESPACE_KEY, _defaultNs);

      loadTargetType(config, root);

      loadSourceFiles(config, root);

      loadPathCollection(config, root, PRIMITIVE_CATEGORY,
         ProjectOption::Primitives, configPath);
      loadPathCollection(config, root, REFERENCE_CATEGORY,
         ProjectOption::References, configPath);

      loadForwards(config, root, FORWARD_CATEGORY);

      loadKeyCollection(config, root, EXTERNAL_CATEGORY,
         ProjectOption::Externals, ProjectOption::External, nullptr);
      loadKeyCollection(config, root, WINAPI_CATEGORY,
         ProjectOption::Winapis, ProjectOption::Winapi, nullptr);

      loadPathSetting(config, root, LIB_PATH, ProjectOption::LibPath, configPath);
      loadPathSetting(config, root, OUTPUT_PATH, ProjectOption::OutputPath, configPath);
      loadPathSetting(config, root, TARGET_PATH, ProjectOption::TargetPath, configPath);

      copySetting(config, root, MGSIZE_PATH, ProjectOption::GCMGSize);
      copySetting(config, root, YGSIZE_PATH, ProjectOption::GCYGSize);
      copySetting(config, root, DEBUGMODE_PATH, ProjectOption::DebugMode);

      copySetting(config, root, FILE_PROLOG, ProjectOption::Prolog, true);
      copySetting(config, root, FILE_EPILOG, ProjectOption::Epilog, true);
   }
}

void Project :: loadConfig(ConfigFile& config, path_t configPath)
{
   ustr_t key = getPlatformName(_platform);

   ConfigFile::Node root = config.selectRootNode();

   // select platform configuration
   ConfigFile::Node platformRoot = config.selectNode<ustr_t>(PLATFORM_CATEGORY, key, [](ustr_t key, ConfigFile::Node& node)
   {
      return node.compareAttribute("key", key);
   });

   // load common project settings
   loadConfig(config, configPath, root);
   // load common target dependent settings
   loadConfig(config, configPath, platformRoot);
}

void Project :: loadDefaultConfig()
{
   ProjectNode node = _root.findChild(ProjectOption::Templates);
   ProjectNode templateNode = node.findChild(ProjectOption::FileKey);
   if (templateNode != ProjectOption::None) {
      PathString templatePath(_paths.get(templateNode.arg.value));

      loadConfig(*templatePath, false);
   }
}

bool Project :: loadConfig(path_t path, bool mainConfig)
{
   PathString configPath;
   configPath.copySubPath(path, false);

   ConfigFile config;
   if (config.load(path, _encoding)) {
      if (mainConfig) {
         _projectPath.copy(*configPath);

         _loaded = true;
      }

      loadConfig(config, *configPath);

      return true;
   }
   else {
      _presenter->printPath(_presenter->getMessage(wrnInvalidConfig), path);

      return false;
   }
}

bool Project :: loadProject(path_t path)
{
   if (_projectName.empty()) {
      if(loadConfig(path, true)) {
         FileNameString fileName(path);
         IdentifierString name(*fileName);

         _projectName.copy(name.str());

         return true;
      }
      else return false;      
   }
   else {
      _presenter->printPath(_presenter->getMessage(errProjectAlreadyLoaded), path);

      return false;
   }
}

void Project :: prepare()
{
   if (!_loaded) {
      loadDefaultConfig();
      _loaded = true;
   }

   if (_defaultNs.empty())
      _defaultNs.copy(*_projectName);

   path_t target = PathSetting(ProjectOption::TargetPath);
   if (target.empty()) {
      PathString path(*_projectPath, _projectName.str());

      addPathSetting(ProjectOption::TargetPath, *path);
   }
}
