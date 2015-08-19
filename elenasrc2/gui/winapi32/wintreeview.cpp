//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI ListView Implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "wintreeview.h"

using namespace _GUI_;

// --- TreeView ---

TreeView :: TreeView(Control* owner, bool persistentSelection)
   : Control(0, 0, 800, 20) // !! temporal
{
   HINSTANCE instance = ((Control*)owner)->_getInstance();

   int styles = WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS;
   if (persistentSelection)
      styles |= TVS_SHOWSELALWAYS;

   _handle = ::CreateWindowEx(
      0, WC_TREEVIEW, NULL, styles,
      _left, _top, _width, _height, owner->getHandle(), NULL, instance, (LPVOID)this);
}

bool TreeView :: isExpanded(TreeViewItem parent)
{
   int state = TreeView_GetItemState(_handle, parent, TVIS_EXPANDEDONCE);

   return _ELENA_::test(state, TVIS_EXPANDEDONCE);
}

TreeViewItem TreeView :: getCurrent()
{
   return TreeView_GetSelection(_handle);
}

TreeViewItem TreeView :: getChild(TreeViewItem parent)
{
   return TreeView_GetChild(_handle, parent);
}

TreeViewItem TreeView :: getNext(TreeViewItem item)
{
   return TreeView_GetNextItem(_handle, item, TVGN_NEXT);
}

int TreeView :: getParam(TreeViewItem item)
{
   TVITEM itemRec; 
    
   itemRec.mask = TVIF_PARAM;
   itemRec.hItem = item;
   itemRec.lParam = -1;

   TreeView_GetItem(_handle, &itemRec);

   return itemRec.lParam;
}

void TreeView :: getCaption(TreeViewItem item, wchar_t* caption, int length)
{
   TVITEM itemRec;

   itemRec.mask = TVIF_TEXT;
   itemRec.hItem = item;
   itemRec.cchTextMax = length;
   itemRec.pszText = caption;

   TreeView_GetItem(_handle, &itemRec);
}

void TreeView :: setParam(TreeViewItem item, int param)
{
   TVITEM itemRec; 
    
   itemRec.mask = TVIF_PARAM;
   itemRec.hItem = item;
   itemRec.lParam = param;

   TreeView_SetItem(_handle, &itemRec);
}

void TreeView :: setCaption(TreeViewItem item, const wchar_t* caption)
{
   TVITEM itemRec;

   itemRec.mask = TVIF_TEXT;
   itemRec.hItem = item;
   itemRec.cchTextMax = _ELENA_::getlength(caption);
   itemRec.pszText = (wchar_t*)caption;

   TreeView_SetItem(_handle, &itemRec);
}

void TreeView :: expand(TreeViewItem item)
{
   TreeView_Expand(_handle, item, TVE_EXPAND);
}

void TreeView :: select(TreeViewItem item)
{
   TreeView_SelectItem(_handle, item); 
}

void TreeView :: collapse(TreeViewItem item)
{
   TreeView_Expand(_handle, item, TVE_COLLAPSE);
}

TreeViewItem TreeView :: insertTo(TreeViewItem parent, const wchar_t* caption, int param)
{
   TV_INSERTSTRUCT item;

   item.hParent=parent;
   item.hInsertAfter= parent ? TVI_LAST : TVI_ROOT;
   item.item.mask= TVIF_TEXT | TVIF_PARAM;
   item.item.pszText=(wchar_t*)caption;
   item.item.lParam = param;

   return (TreeViewItem)::SendMessage(_handle, TVM_INSERTITEM, 0, (LPARAM)&item);
}

void TreeView :: clear(TreeViewItem parent)
{
   HTREEITEM item = TreeView_GetChild(_handle, parent);
   while (item) {
      HTREEITEM next = TreeView_GetNextSibling(_handle, item);
      TreeView_DeleteItem(_handle, item);

      item = next;
   }
}

void TreeView :: erase(TreeViewItem item)
{
   TreeView_DeleteItem(_handle, item);
}
