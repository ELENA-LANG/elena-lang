//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TabBar Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "idecommon.h"
#include "wintabbar.h"
#include "wincanvas.h"

using namespace elena_lang;

// --- CustomTabBar ---

CustomTabBar :: CustomTabBar(NotifierBase* notifier, bool withAbovescore)
   : ControlBase(nullptr)
{
   _notifier = notifier;
   _withAbovescore = withAbovescore;
   _notSelected = true;
}

void CustomTabBar :: onDrawItem(DRAWITEMSTRUCT* item)
{
   Canvas    canvas(item->hDC);
   Rectangle rect(item->rcItem.left, item->rcItem.top, item->rcItem.right - item->rcItem.left + 1, item->rcItem.bottom - item->rcItem.top + 1);

   // For some bizarre reason the rcItem you get extends above the actual
   // drawing area. We have to workaround this "feature".
   rect.bottomRight.y += ::GetSystemMetrics(SM_CYEDGE);

   int  index = item->itemID;
   bool isSelected = (index == getCurrentIndex());

   wchar_t label[0x51];
   TCITEM tci;
   tci.mask = TCIF_TEXT;
   tci.pszText = label;
   tci.cchTextMax = 0x50;
   ::SendMessage(_handle, TCM_GETITEM, index, (LPARAM)&tci);

   //const wchar_t* label = getTabName(index);

   canvas.fillRectangle(rect, Canvas::ButtonFace());
   if (isSelected) {
      if (_withAbovescore) {
         Rectangle barRect(rect);
         barRect.bottomRight.y = 6;

         canvas.fillRectangle(barRect, Color(255, 190, 128));
      }
   }
   else {
      canvas.fillRectangle(rect, Color(220, 220, 220));
   }

   canvas.setTransparentMode(true);

   if (isSelected) {
      //rect.topLeft.y -= ::GetSystemMetrics(SM_CYEDGE);
      rect.topLeft.y += 1;

      canvas.drawText(rect, label, getlength_int(label), Color(0, 0, 0), true);
   }
   else canvas.drawText(rect, label, getlength_int(label), Color(128, 128, 128), true);
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

void CustomTabBar :: selectTab(int index)
{
   int previous = (int)::SendMessage(_handle, TCM_SETCURSEL, index, 0);
   if (_notSelected || previous != index) {
      _notifier->notifyModelChange(NOTIFY_CURRENTVIEW_CHANGED, index);

      _notSelected = false;
   }
}

void CustomTabBar :: renameTab(int index, wstr_t title)
{
   // rename tab caption
   TCITEM tie;
   tie.mask = TCIF_TEXT | TCIF_IMAGE;
   tie.iImage = -1;
   tie.pszText = (wchar_t*)title.str();

   ::SendMessage(_handle, TCM_SETITEM, index, (LPARAM)&tie);
}

int CustomTabBar :: getTabCount()
{
   return (int)::SendMessage(_handle, TCM_GETITEMCOUNT, 0, 0);
}

int CustomTabBar :: getCurrentIndex()
{
   return (int)::SendMessage(_handle, TCM_GETCURSEL, 0, 0);
}

// --- MultiTabControl ---

MultiTabControl :: MultiTabControl(NotifierBase* notifier, bool withAbovescore, ControlBase* child)
   : CustomTabBar(notifier, withAbovescore)
{
   _child = child;
}

void MultiTabControl :: show()
{
   ControlBase::show();
   refresh();
}

void MultiTabControl :: setRectangle(Rectangle rec)
{
   CustomTabBar::setRectangle(rec);

   if (_child) {
      rec.topLeft.y += 32;
      rec.topLeft.x += 4;
      rec.setWidth(rec.width() - 8);
      rec.setHeight(rec.height() - 32);
      _child->setRectangle(rec);
   }
}

void MultiTabControl :: onSetFocus()
{
   if (_child) {
      _child->setFocus();
   }
}

void MultiTabControl :: setFocus()
{
   if (_child->visible()) {
      _child->setFocus();  
   }
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

void MultiTabControl :: renameTabView(int index, wstr_t title)
{
   // rename tab caption
   renameTab(index, title);
}
