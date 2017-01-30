//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI ToolBar Implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "wintoolbar.h"

using namespace _GUI_;

// --- ToolBar ---

ToolBar :: ToolBar(Window* owner, int iconSize, int count, ToolBarButton* buttons)
   : Control(0, 0, 800, iconSize + 11) // !! temporal
{
   HINSTANCE instance = ((Window*)owner)->_getInstance();

   _handle = ::CreateWindowEx(
      WS_EX_PALETTEWINDOW, TOOLBARCLASSNAME, _T("Toolbar"), 
      WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | TBSTYLE_TOOLTIPS |TBSTYLE_FLAT | CCS_TOP | BTNS_AUTOSIZE,
      _left, _top, _width, _height, owner->getHandle(), NULL, instance, (LPVOID)this);

   ::SendMessage(_handle, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);

   //::SendMessage(_self, TB_LOADIMAGES, IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL);
   TBADDBITMAP bitmap = {instance, 0};
   TBBUTTON* tbButtons = new TBBUTTON[count];
   for (int i = 0 ; i < count ; i++) {
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
   ::SendMessage(_handle, TB_SETBUTTONSIZE, 0, MAKELONG(iconSize, iconSize));
   ::SendMessage(_handle, TB_ADDBUTTONS, (WPARAM)count, (LPARAM)tbButtons);
   ::SendMessage(_handle, TB_AUTOSIZE, 0, 0);

   delete[] tbButtons;
}

void ToolBar :: enableItemById(int id, bool doEnable)
{
   TBBUTTONINFO info;
   info.cbSize = sizeof(TBBUTTONINFO);
   info.dwMask = TBIF_STATE;
   info.fsState = (BYTE)(doEnable ? TBSTATE_ENABLED : 0);

   ::SendMessage(_handle, TB_SETBUTTONINFO, id, (LPARAM)&info);
}
