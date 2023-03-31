//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Menu Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "winmenu.h"

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
