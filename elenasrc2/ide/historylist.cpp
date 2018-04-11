//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      MenuHistoryList class implementation
//                                              (C)2005-2018, by Alexei Rakov
//---------------------------------------------------------------------------

#include "historylist.h"
#include "elena.h"

using namespace _GUI_;
using namespace _ELENA_;

RecentList :: RecentList(int maxCount, int menuBaseId)
   : MenuHistoryList(maxCount, menuBaseId, true)
{
}

void RecentList :: load(XmlConfigFile& file, const char* section)
{
   _ConfigFile::Nodes nodes;
   file.select(section, nodes);

   for (auto it = nodes.start(); !it.Eof(); it++) {
      _list.add(text_str(TextString((*it).Content())).clone());
   }
}

void RecentList :: save(XmlConfigFile& file, const char* section)
{
   //file.clear(section);
   for(List<text_c*>::Iterator it = _list.start() ; !it.Eof() ; it++) {
      IdentifierString value(*it);

      //   config.setSetting(section, it.key(), value);
      file.appendSetting(section, value.c_str());
   }
}
