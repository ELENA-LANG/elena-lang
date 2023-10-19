//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Window list header File
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "windowlist.h"

#include "windows/Resource.h"
#include <tchar.h>

using namespace elena_lang;

// --- WindowList ---

WindowList :: WindowList(IDEController* controller, TextViewModelBase* model)
   : MenuHistoryBase(model, 9, IDM_WINDOW_WINDOWS, _T("none"), true)
{
   _controller = controller;
}

void WindowList :: onDocumentSelect(int docIndex)
{
   for (int i = 1; i <= _count; i++) {
      _menu->checkMenuItemById(_menuBaseId + i, false);
   }

   int listIndex = getListIndex(docIndex);
   if (listIndex > 0)
      _menu->checkMenuItemById(_menuBaseId + listIndex, true);
}

void WindowList :: onDocumentNew(int docIndex)
{
   for (int i = 1; i <= _count; i++) {
      _menu->checkMenuItemById(_menuBaseId + i, false);
   }

   MenuHistoryBase::onDocumentNew(docIndex);

   int listIndex = getListIndex(docIndex);
   if (listIndex > 0)
      _menu->checkMenuItemById(_menuBaseId + listIndex, true);
}

void WindowList :: beforeDocumentClose(int docIndex)
{
   MenuHistoryBase::beforeDocumentClose(docIndex);
}

bool WindowList :: select(int listIndex)
{
   if (listIndex <= 0 || _list.count_int() < listIndex)
      return false;

   path_t path = _list.get(listIndex);
   if (!path.empty()){
      _controller->doSelectWindow(_model, path);

      return true;
   }

   return false;
}

// --- RecentList ---

RecentList :: RecentList(IDEController* controller, IDEModel* model, int menuBaseId)
   : MenuHistoryBase(&model->sourceViewModel, 9, menuBaseId, _T("none"), true)
{
   _controller = controller;
   _recentList = nullptr;
}

void RecentList :: onDocumentClose(int)
{
   
}

void RecentList :: onDocumentModeChanged(int, bool)
{
   
}

void RecentList :: onDocumentNew(int index)
{
   
}

void RecentList :: onDocumentSelect(int)
{
   
}

void RecentList :: reload()
{
   _list.clear();

   if (_recentList != nullptr) {
      for (auto it = _recentList->start(); !it.eof(); ++it) {
         _list.add((*it).clone());
      }

      reloadList();
   }
}

void RecentList :: assignList(ProjectPaths* recentList)
{
   _recentList = recentList;

   reload();
}

path_t RecentList :: getPath(int index)
{
   return _list.get(index);
}
