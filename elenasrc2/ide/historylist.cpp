//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      MenuHistoryList class implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "historylist.h"
#include "elena.h"

using namespace _GUI_;
using namespace _ELENA_;

RecentList :: RecentList(int maxCount, int menuBaseId)
   : MenuHistoryList(maxCount, menuBaseId, true)
{
}

void RecentList :: load(IniConfigFile& file, const char* section)
{
   for(ConfigCategoryIterator it = file.getCategoryIt(section) ; !it.Eof() ; it++) {
      _list.add(TextString(it.key()).clone());
   }
}

void RecentList :: save(IniConfigFile& file, const char* section)
{
   file.clear(section);
   for(List<text_c*>::Iterator it = _list.start() ; !it.Eof() ; it++) {
      file.setSetting(section, _ELENA_::IdentifierString(*it), (const char*)NULL);
   }
}
