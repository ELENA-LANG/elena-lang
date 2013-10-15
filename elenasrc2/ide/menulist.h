//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      MenuList class header
//                                              (C)2005-2011, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef menulistH
#define menulistH

#include "ide.h"

namespace _GUI_
{

// --- MenuHistoryList ---

class MenuHistoryList
{
protected:
   int    _maxCount;

   Menu*  _menu;
   int    _menuBaseId;
   size_t _menuSize;

   bool   _withSeparator;

   _ELENA_::List<_text_t*> _list;

   int getIndex(const _text_t* item);
   bool erase(const _text_t* item);
   void eraseLast();

public:
   void assign(Menu* menu)
   {
      _menu = menu;
   }

   const _text_t* get(int id);
   void add(const _text_t* item);

   void refresh();

   void clear();

   MenuHistoryList(int maxCount, int menuBaseId, bool withSeparator);
};

} // _GUI_

#endif // menulistH
