//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      MenuHistoryList class header
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef historylistH
#define historylistH

#include "ide.h"
#include "menulist.h"

namespace _GUI_
{

// --- MenuHistoryList ---

class RecentList : public MenuHistoryList
{
public:
   void load(_ELENA_::IniConfigFile& file, const char* section);
   void save(_ELENA_::IniConfigFile& file, const char* section);

   RecentList(int maxCount, int menuBaseId);
};

} // _GUI_

#endif // historylistH
