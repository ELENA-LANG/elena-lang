//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA command-Line Compiler
//
//		This file contains the project class body
//
//                                             (C)2021-2023, by Aleksey Rakov
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

path_t Project :: PathSetting(ProjectOption option) const
{
   switch (option) {
      case ProjectOption::BasePath:
         return *_basePath;
      case ProjectOption::ProjectPath:
         return *_projectPath;
      case ProjectOption::TargetPath:
      case ProjectOption::OutputPath:
      case ProjectOption::LibPath:
         return XmlProjectBase::PathSetting(option);
      default:
         return nullptr;
   }
}

path_t Project::PathSetting(ProjectOption option, ustr_t key) const
{
   return XmlProjectBase::PathSetting(option, key);
}

ustr_t Project :: StringSetting(ProjectOption option) const
{
   switch (option) {
      case ProjectOption::Namespace:
         return *_defaultNs;
      default:
         return XmlProjectBase::StringSetting(option);
   }
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

PlatformType Project :: SystemTarget()
{
   return _platform & PlatformType::TargetMask;
}

PlatformType Project :: Platform()
{
   return _platform & PlatformType::PlatformMask;
}

PlatformType Project :: TargetType()
{
   return _platform & PlatformType::TargetTypeMask;
}

PlatformType Project :: UITargetType()
{
   return _platform & PlatformType::UIMask;
}

PlatformType Project :: ThreadModeType()
{
   return _platform & PlatformType::ThreadMask;
}

void Project :: addSource(ustr_t ns, path_t path, ustr_t target)
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

   ProjectNode fileNode = current.appendChild(ProjectOption::FileKey, _paths.count() + 1);
   if (!target.empty())
      fileNode.appendChild(ProjectOption::Target, target);

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
   DynamicString<char> target;

   ConfigFile::Collection modules;
   if (config.select(configRoot, MODULE_CATEGORY, modules)) {
      for (auto m_it = modules.start(); !m_it.eof(); ++m_it) {
         ConfigFile::Node moduleNode = *m_it;

         if (!moduleNode.readAttribute("name", subNs)) {
            subNs.clear();
         }
         if (!moduleNode.readAttribute("target", target)) {
            target.clear();
         }

         ReferenceName ns(Namespace(), subNs.str());
         ConfigFile::Collection files;
         if (config.select(moduleNode, "*", files)) {
            for (auto it = files.start(); !it.eof(); ++it) {
               // add source file
               ConfigFile::Node node = *it;
               node.readContent(path);

               PathString filePath(path.str());
               addSource(*ns, *filePath, target.str());
            }
         }
      }
   }
}

void Project :: loadParserTargets(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath)
{
   DynamicString<char> name;
   DynamicString<char> type;
   DynamicString<char> option;

   ConfigFile::Collection targets;
   if (config.select(configRoot, xpath, targets)) {
      for (auto m_it = targets.start(); !m_it.eof(); ++m_it) {
         ConfigFile::Node targetNode = *m_it;

         if (!targetNode.readAttribute("name", name)) {
            name.clear();
         }
         if (!targetNode.readAttribute("type", type)) {
            type.clear();
         }

         ProjectNode node = _root.appendChild(ProjectOption::ParserTargets, name.str());
         node.appendChild(ProjectOption::TargetType, type.str());

         ConfigFile::Collection options;
         if (config.select(targetNode, "*", options)) {
            for (auto it = options.start(); !it.eof(); ++it) {
               ConfigFile::Node optionNode = *it;
               optionNode.readContent(option);

               node.appendChild(ProjectOption::TargetOption, option.str());
            }
         }
      }
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

void Project :: loadTargetType(ConfigFile& config, ConfigFile::Node& root)
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

      loadParserTargets(config, root, PARSER_TARGET_CATEGORY);

      copySetting(config, root, MANIFEST_NAME, ProjectOption::ManifestName);
      copySetting(config, root, MANIFEST_VERSION, ProjectOption::ManifestVersion);
      copySetting(config, root, MANIFEST_AUTHOR, ProjectOption::ManifestAuthor);

      copySetting(config, root, MGSIZE_PATH, ProjectOption::GCMGSize);
      copySetting(config, root, YGSIZE_PATH, ProjectOption::GCYGSize);
      copySetting(config, root, DEBUGMODE_PATH, ProjectOption::DebugMode);
      copySetting(config, root, THREAD_COUNTER, ProjectOption::ThreadCounter);

      copySetting(config, root, FILE_PROLOG, ProjectOption::Prolog, true);
      copySetting(config, root, FILE_EPILOG, ProjectOption::Epilog, true);
   }
}

void Project :: loadConfig(ConfigFile& config, path_t configPath)
{
   ConfigFile::Node root = config.selectRootNode();
   ConfigFile::Node platformRoot = getPlatformRoot(config, _platform);

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

void Project :: setBasePath(path_t path)
{
   _projectPath.copy(path);

   addPathSetting(ProjectOption::OutputPath, path);
}

void Project :: prepare()
{
   if (!_loaded) {
      loadDefaultConfig();
      _loaded = true;
   }

   if (_defaultNs.empty())
      _defaultNs.copy(*_projectName);

   path_t outputPath = PathSetting(ProjectOption::OutputPath);
   if (emptystr(outputPath)) {
      PathString outputPath(_projectPath);

      addPathSetting(ProjectOption::OutputPath, *outputPath);
   }

   path_t target = PathSetting(ProjectOption::TargetPath);
   if (target.empty()) {
      PathString path(*_projectPath, _projectName.str());

      addPathSetting(ProjectOption::TargetPath, *path);
   }
}

void Project :: forEachForward(void* arg, void (* feedback)(void* arg, ustr_t key, ustr_t value))
{
   // list forwards
   for (auto f_it = _forwards.start(); !f_it.eof(); ++f_it) {
      feedback(arg, f_it.key(), *f_it);
   }
}
