//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Project
//
//		This file contains the xml project base class implementation
//
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "xmlprojectbase.h"
#include "langcommon.h"

using namespace elena_lang;

// --- XmlProjectBase ---

ustr_t XmlProjectBase::StringSetting(ProjectOption option) const
{
   ProjectNode node = _root.findChild(option);

   return node.identifier();
}

path_t XmlProjectBase :: PathSetting(ProjectOption option) const
{
   int fileIndex = _root.findChild(option).arg.value;

   return _paths.get(fileIndex);
}


path_t XmlProjectBase :: PathSetting(ProjectOption option, ustr_t key) const
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

bool XmlProjectBase::BoolSetting(ProjectOption option, bool defValue) const
{
   ustr_t val = StringSetting(option);
   if (val.empty())
      return defValue;

   return val == "-1";
}

int XmlProjectBase::IntSetting(ProjectOption option, int defValue) const
{
   ustr_t val = StringSetting(option);
   if (!val.empty()) {
      int intValue = StrConvertor::toInt(val, 10);

      return intValue;
   }
   else return defValue;
}

unsigned int XmlProjectBase :: UIntSetting(ProjectOption option, unsigned int defValue) const
{
   ustr_t val = StringSetting(option);
   if (!val.empty()) {
      unsigned int intValue = StrConvertor::toUInt(val, 10);

      return intValue;
   }
   else return defValue;
}

XmlProjectBase :: XmlProjectBase(PlatformType platform)
   : _paths(nullptr), _forwards(nullptr), _variables(false), _lexicals(nullptr)
{
   _platform = platform;

   ProjectTree::Writer writer(_projectTree);
   writer.newNode(ProjectOption::Root);

   _root = writer.CurrentNode();
}

ConfigFile::Node XmlProjectBase :: getPlatformRoot(ConfigFile& config, PlatformType platform)
{
   ustr_t key = getPlatformName(platform);

   // select platform configuration
   ConfigFile::Node platformRoot = config.selectNode<ustr_t>(PLATFORM_CATEGORY, key, [](ustr_t key, ConfigFile::Node& node)
      {
         return node.compareAttribute("key", key);
      });

   return platformRoot;
}

ConfigFile::Node XmlProjectBase :: getProfileRoot(ConfigFile& config, ConfigFile::Node& root, ustr_t profileName)
{
   ConfigFile::Node profileRoot = config.selectNode<ustr_t>(root, PROFILE_CATEGORY, profileName, [](ustr_t key, ConfigFile::Node& node)
      {
         return node.compareAttribute("key", key);
      });

   return profileRoot;
}

void XmlProjectBase :: loadPathCollection(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath,
   ProjectOption collectionKey, path_t configPath)
{
   DynamicString<char> path;

   ConfigFile::Collection collection;
   if (config.select(configRoot, xpath, collection)) {
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

void XmlProjectBase :: loadPathSetting(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath,
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

void XmlProjectBase :: loadBoolSetting(ConfigFile& config, ConfigFile::Node& configRoot, ustr_t xpath,
   ProjectOption key)
{
   auto configNode = config.selectNode(configRoot, xpath);
   if (!configNode.isNotFound()) {
      DynamicString<char> strValue;
      configNode.readContent(strValue);

      bool value = ustr_t(strValue.str()).compare("-1");

      ProjectNode node = _root.findChild(key);
      if (node == ProjectOption::None) {
         _root.appendChild(key, value ? "-1" : "0");
      }
      else node.setStrArgument(value ? "-1" : "0");
   }
}

void XmlProjectBase :: loadKeyCollection(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath, ProjectOption collectionKey,
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
            else valueNode.setStrArgument(value.str());
         }
      }
   }
}

void XmlProjectBase :: loadForwards(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath)
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

void XmlProjectBase :: loadVariables(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath)
{
   DynamicString<char> key, value;

   ConfigFile::Collection collection;
   if (config.select(root, xpath, collection)) {
      for (auto it = collection.start(); !it.eof(); ++it) {
         ConfigFile::Node node = *it;

         if (node.readAttribute("key", key)) {
            node.readContent(value);

            addVariable(key.str(), ustr_t(value.str()).compare("-1"));
         }
      }
   }
}

void XmlProjectBase :: loadLexicals(ConfigFile& config, ConfigFile::Node& root, ustr_t xpath)
{
   DynamicString<char> key, value;

   ConfigFile::Collection collection;
   if (config.select(root, xpath, collection)) {
      for (auto it = collection.start(); !it.eof(); ++it) {
         ConfigFile::Node node = *it;

         if (node.readAttribute("key", key)) {
            node.readContent(value);

            _lexicals.add(key.str(), ustr_t(value.str()).clone());
         }
      }
   }
}

ustr_t XmlProjectBase :: resolveKey(ProjectOption category, ProjectOption item, ustr_t key)
{
   ProjectNode current = ProjectTree::gotoChild(_root.findChild(category), item, key);

   return current == item ? current.findChild(ProjectOption::Value).identifier() : nullptr;
}

ustr_t XmlProjectBase :: resolveForward(ustr_t forward)
{
   return _forwards.get(forward);
}

ustr_t XmlProjectBase :: resolveWinApi(ustr_t dllAlias)
{
   return resolveKey(ProjectOption::Winapis, ProjectOption::Winapi, dllAlias);
}

ustr_t XmlProjectBase :: resolveExternal(ustr_t dllAlias)
{
   return resolveKey(ProjectOption::Externals, ProjectOption::External, dllAlias);
}

void XmlProjectBase :: addForward(ustr_t forward, ustr_t referenceName)
{
   freeUStr(_forwards.exclude(forward));

   _forwards.add(forward, referenceName.clone());
}

void XmlProjectBase :: addVariable(ustr_t name, bool value)
{
   _variables.erase(name);
   _variables.add(name, value);
}

bool XmlProjectBase :: checkVariable(ustr_t name)
{
   return _variables.get(name);
}