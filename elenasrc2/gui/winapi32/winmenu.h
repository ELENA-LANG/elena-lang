//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Menu Header File
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winmenuH
#define winmenuH

#include "wincommon.h"

namespace _GUI_
{

// --- BaseMenu ---

class BaseMenu : public _BaseControl
{
protected:
   HMENU  _hMenu;

public:
   virtual bool checkHandle(void* param) const
   {
      return (_hMenu == (HMENU)param);
   }

   void enableItemById(int id, bool doEnable) const;
   void enableItemByIndex(int index, bool doEnable) const;

   void checkItemById(int id, bool checked);

   void insertItemById(int positionId, int id, const wchar_t* caption);
   void insertItemByIndex(int index, int command, const wchar_t* caption);

   void insertSeparatorById(int positionId, int id);

   void renameItemById(int id, const wchar_t* caption);

   void eraseItemById(int id);
};

// --- ContextMenu ---

struct MenuInfo
{
   size_t key;
   wchar_t* text;
};

class ContextMenu : public BaseMenu
{
public:
   bool isLoaded() const { return _hMenu != NULL; }

   void create(int count, MenuInfo* items);

   void show(HWND parent, Point& p) const;

   ContextMenu();
   ~ContextMenu();
};

// --- Menu ---

class Menu : public BaseMenu
{
   HACCEL _accel;

public:
   HACCEL getAccel() { return _accel; }

   wchar_t getMnemonicAccKey() const { return '&'; };

   bool _translate(HWND hWnd, LPMSG msg);

   Menu(HINSTANCE hInstance, int resource, HMENU hMenu);
};

} // _GUI_

#endif // winmenuH
