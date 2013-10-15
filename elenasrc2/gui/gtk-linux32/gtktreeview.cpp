//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI ListView Implementation
//                                              (C)2005-2011, by Alexei Rakov
//---------------------------------------------------------------------------
/*
#include "gtktreeview.h"

using namespace _GUI_;

// --- TreeView ---

TreeView :: TreeView(Control* owner)
//   : Control(0, 0, 800, 20) // !! temporal
{
//   HINSTANCE instance = ((Control*)owner)->_getInstance();
//
//   _handle = ::CreateWindowEx(
//      0, WC_TREEVIEW, NULL, WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
//      _left, _top, _width, _height, owner->getHandle(), NULL, instance, (LPVOID)this);
}

bool TreeView :: isExpanded(TreeViewItem parent)
{
//   int state = TreeView_GetItemState(_handle, parent, TVIS_EXPANDEDONCE);
//
//   return _ELENA_::test(state, TVIS_EXPANDEDONCE);

   return false; // !! temporal
}

TreeViewItem TreeView :: getCurrent()
{
   //return TreeView_GetSelection(_handle);
   return NULL; // !! temporal
}

TreeViewItem TreeView :: getChild(TreeViewItem parent)
{
   //return TreeView_GetChild(_handle, parent);
   return NULL; // !! temporal
}

TreeViewItem TreeView :: getNext(TreeViewItem item)
{
   //return TreeView_GetNextItem(_handle, item, TVGN_NEXT);
   return NULL; // !! temporal
}

int TreeView :: getParam(TreeViewItem item)
{
//   TVITEM itemRec;
//
//   itemRec.mask = TVIF_PARAM;
//   itemRec.hItem = item;
//   itemRec.lParam = -1;
//
//   TreeView_GetItem(_handle, &itemRec);
//
//   return itemRec.lParam;
   return 0;
}

void TreeView :: getCaption(TreeViewItem item, TCHAR* caption, int length)
{
//   TVITEM itemRec;
//
//   itemRec.mask = TVIF_TEXT;
//   itemRec.hItem = item;
//   itemRec.cchTextMax = length;
//   itemRec.pszText = caption;
//
//   TreeView_GetItem(_handle, &itemRec);
}

void TreeView :: setParam(TreeViewItem item, int param)
{
//   TVITEM itemRec;
//
//   itemRec.mask = TVIF_PARAM;
//   itemRec.hItem = item;
//   itemRec.lParam = param;
//
//   TreeView_SetItem(_handle, &itemRec);
}

void TreeView :: setCaption(TreeViewItem item, const TCHAR* caption)
{
//   TVITEM itemRec;
//
//   itemRec.mask = TVIF_TEXT;
//   itemRec.hItem = item;
//   itemRec.cchTextMax = _ELENA_::getlength(caption);
//   itemRec.pszText = (TCHAR*)caption;
//
//   TreeView_SetItem(_handle, &itemRec);
}

void TreeView :: expand(TreeViewItem item)
{
//   TreeView_Expand(_handle, item, TVE_EXPAND);
}

void TreeView :: select(TreeViewItem item)
{
//   TreeView_SelectItem(_handle, item);
}

void TreeView :: collapse(TreeViewItem item)
{
//   TreeView_Expand(_handle, item, TVE_COLLAPSE);
}

TreeViewItem TreeView :: insertTo(TreeViewItem parent, const TCHAR* caption, int param)
{
//   TV_INSERTSTRUCT item;
//
//   item.hParent=parent;
//   item.hInsertAfter= parent ? TVI_LAST : TVI_ROOT;
//   item.item.mask= TVIF_TEXT | TVIF_PARAM;
//   item.item.pszText=(TCHAR*)caption;
//   item.item.lParam = param;
//
//   return (TreeViewItem)::SendMessage(_handle, TVM_INSERTITEM, 0, (LPARAM)&item);
   return NULL; // !! temporal
}

void TreeView :: clear(TreeViewItem parent)
{
//   HTREEITEM item = TreeView_GetChild(_handle, parent);
//   while (item) {
//      HTREEITEM next = TreeView_GetNextSibling(_handle, item);
//      TreeView_DeleteItem(_handle, item);
//
//      item = next;
//   }
}

void TreeView :: erase(TreeViewItem item)
{
//   TreeView_DeleteItem(_handle, item);
}
*/