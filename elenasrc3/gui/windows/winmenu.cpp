//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Menu Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "winmenu.h"
#include <tchar.h>

using namespace elena_lang;

// --- MenuBase ---

void MenuBase :: checkItemById(int id, bool checked)
{
   int flag = (checked ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND;

   ::CheckMenuItem(_handle, id, flag);
}

void MenuBase :: enableMenuItemById(int id, bool enable)
{
   int flag = (enable ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)) | MF_BYCOMMAND;

   ::EnableMenuItem(_handle, id, flag);
}

// --- RootMenu ---

RootMenu :: RootMenu(HMENU hMenu)
{
   _handle = hMenu;
}

// --- ContextMenu ---

ContextMenu::ContextMenu()
{
   
}

ContextMenu::~ContextMenu()
{
   if (isLoaded())
      ::DestroyMenu(_handle);
}

void ContextMenu :: create(int count, MenuInfo* items)
{
   _handle = ::CreatePopupMenu();

   for (int i = 0; i < count; i++) {
      if (items[i].key == 0) {
         ::AppendMenu(_handle, MF_SEPARATOR, 0, _T(""));
      }
      else ::AppendMenu(_handle, MF_STRING, items[i].key, items[i].text);
   }

}

void ContextMenu :: show(HWND parent, Point& p) const
{
   ::TrackPopupMenu(_handle, TPM_LEFTALIGN, p.x, p.y, 0, parent, nullptr);
}

