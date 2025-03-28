//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI ToolBar Implementation File
//                                             (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "wintoolbar.h"

#include <tchar.h>

using namespace elena_lang;

// --- ToolBar ---

ToolBar :: ToolBar(int iconSize)
   : ControlBase(_T("Toolbar"), 0, 0, 800, iconSize + 11)
{
   _iconSize = iconSize;
   _hImageList = 0;
}

ToolBar::~ToolBar()
{
   if (_hImageList)
      ImageList_Destroy(_hImageList);
}

HWND ToolBar :: createControl(HINSTANCE instance, ControlBase* owner, 
   ToolBarButton* buttons, size_t counter)
{
   _handle = CreateWindowEx(WS_EX_PALETTEWINDOW, TOOLBARCLASSNAME, _title, 
      WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TOOLTIPS | TBSTYLE_WRAPABLE | TBSTYLE_FLAT | CCS_TOP, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0,
      owner->handle(), nullptr, instance, (LPVOID)this);

   SendMessage(_handle, CCM_SETVERSION, (WPARAM)6, 0);

   ::SendMessage(_handle, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
   ::SendMessage(_handle, TB_SETBITMAPSIZE, 0, MAKELPARAM(_iconSize, _iconSize));

   _hImageList = ImageList_Create(_iconSize, _iconSize,   // Dimensions of individual bitmaps.
      ILC_COLOR24 | ILC_MASK,   // Ensures transparent background.
      2, 0);

   COLORREF crMask = RGB(255, 0, 255);

   TBBUTTON* tbButtons = new TBBUTTON[counter];
   int       imageListId = 0;
   for (size_t i = 0; i < counter/*2*/; i++) {
      tbButtons[i].idCommand = buttons[i].command;
      tbButtons[i].fsState = TBSTATE_ENABLED;
      tbButtons[i].dwData = 0;
      tbButtons[i].iString = 0;

      if (buttons[i].command != 0) {
         tbButtons[i].iBitmap = imageListId++;
         tbButtons[i].fsStyle = TBSTYLE_BUTTON;

         HBITMAP hBitmap = (HBITMAP)LoadImage(instance, MAKEINTRESOURCE(buttons[i].iconId), IMAGE_BITMAP, _iconSize, _iconSize, NULL);

         //ImageList_Add(_hImageList, hBitmap, NULL);
         ImageList_AddMasked(_hImageList, hBitmap, crMask);
      }
      else {
         tbButtons[i].iBitmap = 0;
         tbButtons[i].fsStyle = BTNS_SEP;
      }
   }

   ::SendMessage(_handle, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)_hImageList);
   ::SendMessage(_handle, TB_ADDBUTTONS, counter, (LPARAM)tbButtons);
   ::SendMessage(_handle, TB_AUTOSIZE, 0, 0);

   delete[] tbButtons;

   return _handle;
}

void ToolBar :: enableItemById(int id, bool doEnable)
{
   TBBUTTONINFO info;
   info.cbSize = sizeof(TBBUTTONINFO);
   info.dwMask = TBIF_STATE;
   info.fsState = (BYTE)(doEnable ? TBSTATE_ENABLED : 0);

   ::SendMessage(_handle, TB_SETBUTTONINFO, id, (LPARAM)&info);
}
