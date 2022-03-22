//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TabBar Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "wintabbar.h"

using namespace elena_lang;

// --- CustomTabBar ---

CustomTabBar :: CustomTabBar()
   : ControlBase(nullptr)
{
   
}

void CustomTabBar :: addTab(int index, wstr_t name, void* param)
{
   TCITEM tie;
   tie.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
   tie.iImage = -1;
   tie.pszText = (wchar_t*)name.str();
   tie.lParam = (LPARAM)param;

   ::SendMessage(_handle, TCM_INSERTITEM, index, (LPARAM)&tie);
}

int CustomTabBar :: getTabCount()
{
   return (int)::SendMessage(_handle, TCM_GETITEMCOUNT, 0, 0);
}

// --- MultiTabControl ---

MultiTabControl::MultiTabControl(ControlBase* child)
   : CustomTabBar()
{
   
}

HWND MultiTabControl :: createControl(HINSTANCE instance, ControlBase* owner)
{
   _handle = ::CreateWindowEx(
      WS_EX_CLIENTEDGE, WC_TABCONTROL, _title,
      WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER | TCS_FOCUSNEVER | TCS_TABS | TCS_SINGLELINE | TCS_OWNERDRAWFIXED | TCS_TOOLTIPS,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, owner->handle(), nullptr, instance, (LPVOID)this);

   return _handle;
}

int MultiTabControl :: addTabView(wstr_t title, void* param)
{
   int index = getTabCount();

   addTab(index, title, param);

   return index;
}
