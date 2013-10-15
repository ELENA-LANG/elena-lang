//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      MenuHistoryList class implementation
//                                              (C)2005-2011, by Alexei Rakov
//---------------------------------------------------------------------------

#include "historylist.h"

using namespace _GUI_;
using namespace _ELENA_;

RecentList :: RecentList(int maxCount, int menuBaseId)
   : MenuHistoryList(maxCount, menuBaseId, true)
{
}

void RecentList :: load(IniConfigFile& file, const _text_t* section)
{
   for(ConfigCategoryIterator it = file.getCategoryIt(section) ; !it.Eof() ; it++) {
      _list.add(StringHelper::clone(it.key()));
   }
}

void RecentList :: save(IniConfigFile& file, const _text_t* section)
{
   file.clear(section);
   for(List<_text_t*>::Iterator it = _list.start() ; !it.Eof() ; it++) {
      file.setSetting(section, *it, DEFAULT_STR);
   }
}
