//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI ListView Implementation
//                             (C)2005-2017, by Alexei Rakov, Alexandre Bencz
//---------------------------------------------------------------------------

#include "wintreeview.h"

using namespace _GUI_;

// --- TreeView ---

TreeView :: TreeView(Control* owner, bool persistentSelection, bool enableIcons)
   : Control(0, 0, 800, 20) // !! temporal
{
   HINSTANCE instance = ((Control*)owner)->_getInstance();

   int styles = WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT;
   if (persistentSelection)
      styles |= TVS_SHOWSELALWAYS;

   _handle = ::CreateWindowEx(
      0, WC_TREEVIEW, NULL, styles,
      _left, _top, _width, _height, owner->getHandle(), NULL, instance, (LPVOID)this);
   
   if (enableIcons) {
	   _hImages = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
		   GetSystemMetrics(SM_CYSMICON),
		   ILC_COLOR32 | ILC_MASK, 1, 1);
	   HINSTANCE hLib = LoadLibrary(_T("shell32.dll"));

	   HICON hIcon1 = reinterpret_cast<HICON>(LoadImage(hLib, MAKEINTRESOURCE(4), IMAGE_ICON, 0, 0, LR_SHARED));
	   ImageList_AddIcon(_hImages, hIcon1);
	   FreeLibrary(hLib);

	   HICON hIcon2 = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(IDR_FILETREE), IMAGE_ICON, 0, 0, LR_SHARED));
	   ImageList_AddIcon(_hImages, hIcon2);

	   TreeView_SetImageList(_handle, _hImages, TVSIL_NORMAL);
   }
   else _hImages = NULL;
}

TreeView :: ~TreeView()
{
   if (_hImages)
      ImageList_Destroy(_hImages);
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

   return (int)itemRec.lParam;
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

void TreeView :: setParam(TreeViewItem item, size_t param)
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
   itemRec.cchTextMax = (int)_ELENA_::getlength(caption);
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

TreeViewItem TreeView :: insertTo(TreeViewItem parent, const wchar_t* caption, size_t param, bool isNode)
{
   TV_INSERTSTRUCT item;

   item.hParent=parent;
   item.hInsertAfter= parent ? TVI_LAST : TVI_ROOT;
   item.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
   item.item.pszText=(wchar_t*)caption;
   item.item.lParam = param;
   item.item.iSelectedImage = item.item.iImage = isNode ? 0 : 1;

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
