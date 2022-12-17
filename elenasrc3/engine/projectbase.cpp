//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Project
//
//		This file contains the project base class implementation
//
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "projectbase.h"
#include "langcommon.h"

using namespace elena_lang;

// --- XmlProjectBase ---

ustr_t XmlProjectBase :: StringSetting(ProjectOption option) const
{
   ProjectNode node = _root.findChild(option);

   return node.identifier();
}

path_t XmlProjectBase :: PathSetting(ProjectOption option) const
{
   int fileIndex = _root.findChild(option).arg.value;

   return _paths.get(fileIndex);
}

bool XmlProjectBase :: BoolSetting(ProjectOption option, bool defValue) const
{
   ustr_t val = StringSetting(option);
   if (val.empty())
      return defValue;

   return val == "-1";
}

int XmlProjectBase :: IntSetting(ProjectOption option, int defValue) const
{
   ustr_t val = StringSetting(option);
   if (!val.empty()) {
      int intValue = StrConvertor::toInt(val, 10);

      return intValue;
   }
   else return defValue;
}

XmlProjectBase :: XmlProjectBase(PlatformType platform)
   : _paths(nullptr), _forwards(nullptr)
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
