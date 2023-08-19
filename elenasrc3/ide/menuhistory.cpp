//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE menu history implementation File
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "menuhistory.h"

using namespace elena_lang;

// --- MenuHistory ---

MenuHistoryBase :: MenuHistoryBase(TextViewModelBase* model, 
   int maxCount, int menuBaseId,
   const_text_t defCaption,
   bool withSeparator)
   : _maxCount(maxCount), _count(0), _menuBaseId(menuBaseId),
      _withSeparator(withSeparator),
      _defCaption(defCaption),
      _model(model), _list(nullptr)
{
   _model->attachListener(this);
}

int MenuHistoryBase :: getListIndex(int docIndex)
{
   path_t path = _model->getDocumentPath(docIndex);

   int index = _list.retrieveIndex<path_t>(path, [](path_t arg, path_t current)
      {
         return current.compare(arg);
      });

   return index >= 0 ? index + 1 : -1;
}

void MenuHistoryBase :: reloadList()
{
   if (_count == 0 && _list.count_int() > 0 && _withSeparator) {
      // insert separator before clear command
      _menu->insertSeparatorById(_menuBaseId + _maxCount + 1, _menuBaseId);
      _menu->enableMenuItemById(_menuBaseId + _maxCount + 1, true);
   }

   while (_list.count_int() > _count) {
      _count++;
      _menu->insertMenuItemById(_menuBaseId, _menuBaseId + _count, _defCaption);
   }

   while (_list.count_int() < _count) {
      _menu->eraseMenuItemById(_menuBaseId + _count);
      _count--;
   }

   String<text_c, 255>  caption;
   int index = 1;
   auto it = _list.start();
   while (!it.eof()) {
      caption.clear();

      caption.append(_menu->getMnemonicAccKey());
      caption.appendInt(index);
      caption.append(':');
      caption.append(' ');
      caption.append(*it);

      _menu->renameMenuItemById(index + _menuBaseId, caption.str());

      ++it;
      index++;
   }
}

void MenuHistoryBase :: eraseLast()
{
   _list.cut(_list.end());
}

void MenuHistoryBase :: onDocumentNew(int index)
{
   path_t path = _model->getDocumentPath(index);

   _list.insert(path.clone());

   if (_list.count_int() >= _maxCount)
      eraseLast();

   reloadList();
}

void MenuHistoryBase :: beforeDocumentClose(int index)
{
   path_t path = _model->getDocumentPath(index);

   for (auto it = _list.start(); !it.eof(); ++it) {
      if (path.compare(*it)) {
         _list.cut(it);
         reloadList();

         break;
      }
   }
}
