//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TreeView Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "idecommon.h"
#include "wintreeview.h"

using namespace elena_lang;

// --- TreeView ---

TreeView :: TreeView(int width, int height, bool persistentSelection, bool enableIcons, int iconId)
   : ControlBase(nullptr, 0, 0, width, height),
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
   int styles = WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT;
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
