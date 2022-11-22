//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TreeView Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "idecommon.h"
#include "wintreeview.h"

#include <tchar.h>

using namespace elena_lang;

// --- TreeView ---

TreeView :: TreeView(int width, int height, NotifierBase* notifier, int notificationId, 
   bool persistentSelection, bool enableIcons, int iconId)
   : ControlBase(nullptr, 0, 0, width, height),
   _notifier(notifier), _notificationId(notificationId),
   _persistentSelection(persistentSelection), _enableIcons(enableIcons), _iconId(iconId)
{

}

TreeView :: ~TreeView()
{
   if (_hImages)
      ImageList_Destroy(_hImages);
}

HWND TreeView :: createControl(HINSTANCE instance, ControlBase* owner)
{
   int styles = WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT;
   if (_persistentSelection)
      styles |= TVS_SHOWSELALWAYS;

   _handle = ::CreateWindowEx(
      0, WC_TREEVIEW, _title,
      styles,
      CW_USEDEFAULT, 0, _rect.width(), _rect.height(), owner->handle(), nullptr, instance, (LPVOID)this);

   if (_enableIcons) {
      _hImages = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
         GetSystemMetrics(SM_CYSMICON),
         ILC_COLOR32 | ILC_MASK, 1, 1);
      HINSTANCE hLib = LoadLibrary(_T("shell32.dll"));

      HICON hIcon1 = reinterpret_cast<HICON>(LoadImage(hLib, MAKEINTRESOURCE(4), IMAGE_ICON, 0, 0, LR_SHARED));
      ImageList_AddIcon(_hImages, hIcon1);
      FreeLibrary(hLib);

      if (_iconId) {
         HICON hIcon2 = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(_iconId), IMAGE_ICON, 0, 0, LR_SHARED));
         ImageList_AddIcon(_hImages, hIcon2);
      }

      TreeView_SetImageList(_handle, _hImages, TVSIL_NORMAL);
   }
   else _hImages = nullptr;

   return _handle;
}

void TreeView :: onSelChanged()
{
   size_t param = getParam(getCurrent());

   _notifier->notifyMessage(_notificationId, param);
}

size_t TreeView :: getParam(TreeViewItem item)
{
   TVITEM itemRec;

   itemRec.mask = TVIF_PARAM;
   itemRec.hItem = item;
   itemRec.lParam = -1;

   TreeView_GetItem(_handle, &itemRec);

   return itemRec.lParam;
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

void TreeView :: select(TreeViewItem item)
{
   TreeView_SelectItem(_handle, item);
}

size_t TreeView :: readCaption(TreeViewItem item, wchar_t* caption, size_t length)
{
   TVITEM itemRec;

   itemRec.mask = TVIF_TEXT;
   itemRec.hItem = item;
   itemRec.cchTextMax = (int)length;
   itemRec.pszText = caption;

   TreeView_GetItem(_handle, &itemRec);

   return getlength(caption);
}

void TreeView :: setCaption(TreeViewItem item, wchar_t* caption, size_t length)
{
   TVITEM itemRec;

   itemRec.mask = TVIF_TEXT;
   itemRec.hItem = item;
   itemRec.cchTextMax = (int)length;
   itemRec.pszText = caption;

   TreeView_SetItem(_handle, &itemRec);
}

void TreeView :: collapse(TreeViewItem item)
{
   TreeView_Expand(_handle, item, TVE_COLLAPSE);
}

void TreeView :: expand(TreeViewItem item)
{
   TreeView_Expand(_handle, item, TVE_EXPAND);
}

TreeViewItem TreeView :: insertTo(TreeViewItem parent, const wchar_t* caption, size_t param, bool isNode)
{
   TV_INSERTSTRUCT item;

   item.hParent = parent;
   item.hInsertAfter = parent ? TVI_LAST : TVI_ROOT;
   item.item.mask = TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE;
   item.item.pszText = (wchar_t*)caption;
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
