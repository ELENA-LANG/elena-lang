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
}

HWND ToolBar :: createControl(HINSTANCE instance, ControlBase* owner, 
   ToolBarButton* buttons, size_t counter)
{
   _handle = ::CreateWindowEx(
      WS_EX_PALETTEWINDOW, TOOLBARCLASSNAME, _title,
      WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT | CCS_TOP | BTNS_AUTOSIZE,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, owner->handle(), nullptr, instance, (LPVOID)this);

   ::SendMessage(_handle, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

   //::SendMessage(_self, TB_LOADIMAGES, IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL);
   TBADDBITMAP bitmap = { instance, 0 };
   TBBUTTON* tbButtons = new TBBUTTON[counter];
   for (size_t i = 0; i < counter; i++) {
      tbButtons[i].idCommand = buttons[i].command;
      tbButtons[i].fsState = TBSTATE_ENABLED;
      tbButtons[i].dwData = 0;
      tbButtons[i].iString = 0;

      if (buttons[i].command != 0) {
         bitmap.nID = buttons[i].iconId;
         tbButtons[i].iBitmap = (int)::SendMessage(_handle, TB_ADDBITMAP, 1, (LPARAM)&bitmap);

         tbButtons[i].fsStyle = BTNS_BUTTON;
      }
      else {
         tbButtons[i].iBitmap = 0;
         tbButtons[i].fsStyle = BTNS_SEP;
      }
   }
   ::SendMessage(_handle, TB_SETBUTTONSIZE, 0, MAKELONG(_iconSize, _iconSize));
   ::SendMessage(_handle, TB_ADDBUTTONS, (WPARAM)counter, (LPARAM)tbButtons);
   ::SendMessage(_handle, TB_AUTOSIZE, 0, 0);

   delete[] tbButtons;

   return _handle;
}
