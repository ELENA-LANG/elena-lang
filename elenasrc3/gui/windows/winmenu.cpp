//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Menu Implementation File
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "winmenu.h"
#include <tchar.h>

using namespace elena_lang;

// --- MenuBase ---

void MenuBase :: checkMenuItemById(int id, bool checked)
{
   int flag = (checked ? MF_CHECKED : MF_UNCHECKED) | MF_BYCOMMAND;

   ::CheckMenuItem(_handle, id, flag);
}

void MenuBase :: enableMenuItemById(int id, bool enable)
{
   int flag = (enable ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)) | MF_BYCOMMAND;

   ::EnableMenuItem(_handle, id, flag);
}

void MenuBase :: enableMenuItemByIndex(int index, bool enable)
{
   int flag = (enable ? MF_ENABLED : (MF_DISABLED | MF_GRAYED)) | MF_BYPOSITION;

   ::EnableMenuItem(_handle, index, flag);
}

void MenuBase :: insertMenuItemById(int positionId, int id, const_text_t caption)
{
   MENUITEMINFO mii;

   mii.cbSize = sizeof(MENUITEMINFO);
   mii.fMask = MIIM_ID | MIIM_STRING;
   mii.fType = MFT_STRING;
   mii.wID = id;
   mii.dwTypeData = (wchar_t*)caption;

   ::InsertMenuItem(_handle, positionId, FALSE, &mii);
}

void MenuBase :: insertMenuItemByIndex(int index, int command, const_text_t caption)
{
   MENUITEMINFO mii;

   mii.cbSize = sizeof(MENUITEMINFO);
   mii.fMask = MIIM_ID | MIIM_STRING;
   mii.fType = MFT_STRING;
   mii.wID = command;
   mii.dwTypeData = (wchar_t*)caption;

   ::InsertMenuItem(_handle, index, TRUE, &mii);
}

void MenuBase :: insertSeparatorById(int positionId, int id)
{
   MENUITEMINFO mii;

   mii.cbSize = sizeof(MENUITEMINFO);
   mii.fMask = MIIM_ID;
   mii.fType = MF_MENUBREAK;
   mii.wID = id;

   ::InsertMenuItem(_handle, positionId, FALSE, &mii);
}

void MenuBase :: renameMenuItemById(int id, const_text_t caption)
{
   MENUITEMINFO mii;

   mii.cbSize = sizeof(MENUITEMINFO);
   mii.fMask = MIIM_STRING;
   mii.fType = MFT_STRING;
   mii.dwTypeData = (wchar_t*)caption;

   ::SetMenuItemInfo(_handle, id, FALSE, &mii);
}

void MenuBase :: eraseMenuItemById(int id)
{
   ::DeleteMenu(_handle, id, MF_BYCOMMAND);
}

wchar_t MenuBase :: getMnemonicAccKey()
{
   return '&';
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

