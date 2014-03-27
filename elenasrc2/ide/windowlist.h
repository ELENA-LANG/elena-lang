//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      WindowList class header
//                                              (C)2005-2011, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef windowlistH
#define windowlistH

#include "ide.h"
#include "menulist.h"

namespace _GUI_
{

// --- WindowList ---

class WindowList : public MenuHistoryList
{
public:
   void add(const tchar_t* item);
   void remove(const tchar_t* item);

   WindowList(int maxCount, int menuBaseId);
};

} // _GUI_

#endif // windowlistH
