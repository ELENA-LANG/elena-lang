//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      MenuList class implementation
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "menulist.h"

using namespace _GUI_;
using namespace _ELENA_;

// --- MenuHistoryList

MenuHistoryList :: MenuHistoryList(int maxCount, int menuBaseId, bool withSeparator)
   : _list(NULL, freestr)
{
   _maxCount = maxCount;
   _menuBaseId = menuBaseId;
   _menu = NULL;
   _menuSize = 0;
   _withSeparator = withSeparator;
}

int MenuHistoryList :: getIndex(text_str item)
{
   int index = 0;
   List<text_c*>::Iterator it = _list.start();
   while (!it.Eof()) {
      if (item.compare(*it)) {
         return index;
      }
      index++;
      it++;
   }

   return -1;
}

bool MenuHistoryList :: erase(text_str item)
{
   List<text_c*>::Iterator it = _list.start();
   while (!it.Eof()) {
      if (item.compare(*it)) {
         _list.cut(it);

         return true;
      }
      it++;
   }

   return false;
}

void MenuHistoryList :: eraseLast()
{
   _list.cut(_list.end());
}

text_t MenuHistoryList :: get(int id)
{
   _ELENA_::List<text_c*>::Iterator it = _list.get(id - _menuBaseId - 1);

   return !it.Eof() ? *it : NULL;
}

void MenuHistoryList :: add(text_str item)
{
   text_c* itemCopy = item.clone();

   erase(item);

   _list.insert(itemCopy);

   if ((int)_list.Count() >= _maxCount)
      eraseLast();

   refresh();
}

void MenuHistoryList :: clear()
{
   _list.clear();

   refresh();
   if (_withSeparator) {
      _menu->eraseItemById(_menuBaseId);
      _menu->enableItemById(_menuBaseId + _maxCount + 1, false);
   }
}

void MenuHistoryList :: refresh()
{
   if (_menuSize == 0 && _list.Count() > 0 && _withSeparator) {
      // insert separator before clear command
      _menu->insertSeparatorById(_menuBaseId + _maxCount + 1, _menuBaseId);
      _menu->enableItemById(_menuBaseId + _maxCount + 1, true);
   }

   while ((int)_list.Count() > _menuSize) {
      _menuSize++;
      _menu->insertItemById(_menuBaseId, _menuBaseId + _menuSize, _T("none"));
   }

   while ((int)_list.Count() < _menuSize) {
      _menu->eraseItemById(_menuBaseId + _menuSize);
      _menuSize--;
   }

   Path caption;
   int index = 1;
   List<text_c*>::Iterator it = _list.start();
   while (!it.Eof()) {
      caption.clear();

      caption.append(_menu->getMnemonicAccKey());
      caption.appendInt(index);
      caption.append(_T(": "));
      caption.append(*it);

      _menu->renameItemById(index + _menuBaseId, caption);

      it++;
      index++;
   }
}
