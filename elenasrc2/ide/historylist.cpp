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

void RecentList :: load(IniConfigFile& file, const char* section)
{
   String<_text_t, 255> param;
   for(ConfigCategoryIterator it = file.getCategoryIt(section) ; !it.Eof() ; it++) {
      param.copy(it.key());

      _list.add(StringHelper::clone(param));
   }
}

void RecentList :: save(IniConfigFile& file, const char* section)
{
   String<char, 255> param;

   file.clear(section);
   for(List<_text_t*>::Iterator it = _list.start() ; !it.Eof() ; it++) {
      param.copy(*it);

      file.setSetting(section, param, (const char*)NULL);
   }
}
