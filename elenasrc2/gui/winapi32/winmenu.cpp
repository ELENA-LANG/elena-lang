//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Menu Implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "winmenu.h"

using namespace _GUI_;

// --- BaseMenu ---

void BaseMenu :: enableItemById(int id, bool enable) const
{
   int flag = (enable ? MF_ENABLED:(MF_DISABLED | MF_GRAYED)) | MF_BYCOMMAND;
   
   ::EnableMenuItem(_hMenu, id, flag);
}

void BaseMenu :: enableItemByIndex(int index, bool enable) const
{
   int flag = (enable ? MF_ENABLED:(MF_DISABLED | MF_GRAYED)) | MF_BYPOSITION;
   
   ::EnableMenuItem(_hMenu, index, flag);
}

void BaseMenu :: checkItemById(int id, bool checked)
{
   int flag = (checked ? MF_CHECKED:MF_UNCHECKED) | MF_BYCOMMAND;
   
   ::CheckMenuItem(_hMenu, id, flag);
}


void BaseMenu :: insertItemByIndex(int index, int command, const wchar_t* caption)
{
   MENUITEMINFO mii;  
     
   mii.cbSize = sizeof(MENUITEMINFO);
   mii.fMask = MIIM_ID | MIIM_STRING;
   mii.fType = MFT_STRING;
   mii.wID = command;
   mii.dwTypeData = (wchar_t*)caption;     
     
   ::InsertMenuItem(_hMenu, index, TRUE, &mii);   
}

void BaseMenu :: insertItemById(int positionId, int id, const wchar_t* caption)
{
   MENUITEMINFO mii;  
     
   mii.cbSize = sizeof(MENUITEMINFO);
   mii.fMask = MIIM_ID | MIIM_STRING;
   mii.fType = MFT_STRING;
   mii.wID = id;
   mii.dwTypeData = (wchar_t*)caption;     
     
   ::InsertMenuItem(_hMenu, positionId, FALSE, &mii);   
}

void BaseMenu :: insertSeparatorById(int positionId, int id)
{
   MENUITEMINFO mii;  
     
   mii.cbSize = sizeof(MENUITEMINFO);
   mii.fMask = MIIM_ID;
   mii.fType = MF_MENUBREAK;
   mii.wID = id;
     
   ::InsertMenuItem(_hMenu, positionId, FALSE, &mii);   
}

void BaseMenu :: renameItemById(int id, const wchar_t* caption)
{
   MENUITEMINFO mii;  
     
   mii.cbSize = sizeof(MENUITEMINFO);
   mii.fMask = MIIM_STRING;
   mii.fType = MFT_STRING;
   mii.dwTypeData = (wchar_t*)caption;     
          
   ::SetMenuItemInfo(_hMenu, id, FALSE, &mii);  
}

void BaseMenu :: eraseItemById(int id)
{
   BOOL ret = ::DeleteMenu(_hMenu, id, MF_BYCOMMAND);
}

// --- ContextMenu ---

ContextMenu :: ContextMenu()
{
}

ContextMenu :: ~ContextMenu()
{
   if (isLoaded())
      ::DestroyMenu(_hMenu);
}

void ContextMenu :: create(int count, MenuInfo* items)
{
   _hMenu = ::CreatePopupMenu();

   for (int i = 0; i < count ; i++) {
	  if (items[i].key==0) {
         ::AppendMenu(_hMenu, MF_SEPARATOR, 0, _T(""));             
	  }
	  else ::AppendMenu(_hMenu, MF_STRING, items[i].key, items[i].text); 
   }
}

void ContextMenu :: show(HWND hParent, Point& p) const 
{
   ::TrackPopupMenu(_hMenu, TPM_LEFTALIGN, p.x, p.y, 0, hParent, NULL);
};

// --- Menu ---

Menu :: Menu(HINSTANCE hInstance, int resource, HMENU hMenu)
{
   _accel = ::LoadAccelerators(hInstance, MAKEINTRESOURCE(resource));
   _hMenu = hMenu;
}

bool Menu :: _translate(HWND hWnd, LPMSG msg)
{
   return ::TranslateAccelerator(hWnd, _accel, msg) != 0;  
}
