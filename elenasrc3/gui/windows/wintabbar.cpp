//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TabBar Implementation File
//                                             (C)2021-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "idecommon.h"
#include "wintabbar.h"
#include "wincanvas.h"

#include <tchar.h>
#include <commctrl.h>
#include <windowsx.h>

using namespace elena_lang;

// --- CustomTabBar ---

CustomTabBar :: CustomTabBar(NotifierBase* notifier, bool withAbovescore, bool withHighlighting, int width, int height)
   : ControlBase(nullptr, 0, 0, width, height), _hImages(nullptr), _highlighted(false), _isTracked(false)
{
   _notifier = notifier;
   _selectionInvoker = nullptr;
   _withAbovescore = withAbovescore;
   _withHighlighting = withHighlighting;
   _notSelected = true;
}

CustomTabBar :: ~CustomTabBar()
{
   if (_hImages)
      ImageList_Destroy(_hImages);
}

bool CustomTabBar :: isOverButton(Point* screenPoint)
{
   POINT pt = { screenPoint->x, screenPoint->y };
   ::ScreenToClient(_handle, &pt);

   RECT rect;

   if (TabCtrl_GetItemRect(_handle, getCurrentIndex(), &rect)) {
      // if it is over the tab
      if (rect.left < pt.x && rect.right > pt.x && rect.top < pt.y && rect.bottom > pt.y) {
         if (rect.right - pt.x <= 14)
            return true;
      }
   }

   return false;
}

bool CustomTabBar :: isOverTab(Point* p)
{
   POINT pt = { p->x, p->y };
   //::ScreenToClient(_handle, &pt);

   RECT rect;
   if (TabCtrl_GetItemRect(_handle, getCurrentIndex(), &rect)) {
      if (pt.y > -3)
         pt.y = rect.top;

      // if it is over the tab
      if (rect.left < pt.x && rect.right > pt.x && rect.top <= pt.y && rect.bottom >= pt.y) {
         return true;
      }
   }

   return false;
}

bool CustomTabBar :: onMouseMove(Point* p)
{
   if (isOverTab(p)) {
      if (!_isTracked) {
         TRACKMOUSEEVENT tme = {};
         tme.cbSize = sizeof(TRACKMOUSEEVENT);
         tme.dwFlags = TME_LEAVE | TME_HOVER;
         tme.hwndTrack = _handle;
         tme.dwHoverTime = 40;
         TrackMouseEvent(&tme);

         _isTracked = true;
      }

      return true;
   }

   return false;
}

void CustomTabBar :: setHighligthed(bool value)
{
   if (value != _highlighted) {
      _highlighted = value;

      int imageId = getImageId();
      if (imageId != -1) {
         TCITEM tie;
         tie.mask = TCIF_IMAGE;
         tie.iImage = imageId;

         ::SendMessage(_handle, TCM_SETITEM, getCurrentIndex(), (LPARAM)&tie);
      }

      invalidate();
   }
}

void CustomTabBar :: onMouseHover()
{
   DWORD dwpos = ::GetMessagePos();
   Point p(GET_X_LPARAM(dwpos), GET_Y_LPARAM(dwpos));

   setHighligthed(isOverButton(&p));
   _isTracked = false;
}

void CustomTabBar::onMouseLeave()
{
   _isTracked = false;
   setHighligthed(false);
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
   tci.mask = TCIF_TEXT | TCIF_IMAGE;
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

      if (_hImages) {
         rect.bottomRight.x -= 10;

         canvas.drawText(rect, label, getlength_int(label), Color(0, 0, 0), true);

         //if (_highlighted) {
         //   Rectangle hrect = { rect.bottomRight.x - 8, rect.topLeft.y + 8, 20, 20 };

         //   canvas./*drawTransparentRectangle*/fillRectangle(hrect, Color(0, 0, 0));
         //}

         ImageList_Draw(_hImages, tci.iImage, item->hDC, rect.bottomRight.x - 8, rect.topLeft.y + 8, ILD_TRANSPARENT);
      }
      else canvas.drawText(rect, label, getlength_int(label), Color(0, 0, 0), true);
   }
   else canvas.drawText(rect, label, getlength_int(label), Color(128, 128, 128), true);
}

void CustomTabBar :: addTab(int index, wstr_t name, void* param, int iconIndex)
{
   TCITEM tie;
   tie.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
   tie.pszText = (wchar_t*)name.str();
   tie.lParam = (LPARAM)param;
   tie.iImage = iconIndex;

   ::SendMessage(_handle, TCM_INSERTITEM, index, (LPARAM)&tie);
}

void CustomTabBar :: selectTab(int index)
{
   int previous = (int)::SendMessage(_handle, TCM_SETCURSEL, index, 0);
   if (_notSelected || previous != index) {
      if (_selectionInvoker)
         _selectionInvoker(_notifier, index);

      _notSelected = false;
   }
}

void CustomTabBar :: renameTab(int index, wstr_t title)
{
   // rename tab caption
   TCITEM tie;
   tie.mask = TCIF_TEXT;
   tie.iImage = -1;
   tie.pszText = (wchar_t*)title.str();

   ::SendMessage(_handle, TCM_SETITEM, index, (LPARAM)&tie);
}

void CustomTabBar :: deleteTab(int index)
{
   ::SendMessage(_handle, TCM_DELETEITEM, index, 0);
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

MultiTabControl :: MultiTabControl(NotifierBase* notifier, bool withAbovescore, bool withHighlighting, ControlBase* child, int iconId, int acticeIconId)
   : CustomTabBar(notifier, withAbovescore, withHighlighting, 50, 50), _iconId(iconId), _acticeIconId(acticeIconId)
{
   _child = child;
}

void MultiTabControl :: show()
{
   ControlBase::show();
   if (_child)
      _child->show();

   refresh();
}

void MultiTabControl :: setRectangle(Rectangle rec)
{
   CustomTabBar::setRectangle(rec);

   if (_child) {
      rec.topLeft.y += 35;
      rec.topLeft.x += 4;
      rec.setWidth(rec.width() - 8);
      rec.setHeight(rec.height() - 6);
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

   if (_iconId != 0) {
      _hImages = ImageList_Create(GetSystemMetrics(SM_CXSMICON),
         GetSystemMetrics(SM_CYSMICON),
         ILC_COLOR32 | ILC_MASK, 2, 1);

      ImageList_SetIconSize(_hImages, 12, 12);

      HICON hIcon = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(_iconId), IMAGE_ICON, 12, 12, LR_SHARED));
      ImageList_AddIcon(_hImages, hIcon);

      HICON hIconActive = reinterpret_cast<HICON>(LoadImage(GetModuleHandle(0), MAKEINTRESOURCE(_acticeIconId), IMAGE_ICON, 12, 12, LR_SHARED));
      ImageList_AddIcon(_hImages, hIconActive);

      TabCtrl_SetImageList(_handle, _hImages);

      ::SetWindowSubclass(_handle, TabBarProc, 1, reinterpret_cast<DWORD_PTR>(this));
   }

   return _handle;
}

LRESULT MultiTabControl :: TabBarProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
   MultiTabControl* window = (MultiTabControl*)dwRefData;

   switch (message) {
      case WM_NCDESTROY:
         ::RemoveWindowSubclass(hWnd, TabBarProc, uIdSubclass);
         break;
      default:
         return window->proceed(message, wParam, lParam);
   }

   return ::DefSubclassProc(hWnd, message, wParam, lParam);
}

LRESULT MultiTabControl :: proceed(UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message) {
      case WM_MOUSEMOVE:
         if (_withHighlighting) {
            Point p = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };

            onMouseMove(&p);
         }
         break;
      case WM_MOUSEHOVER:
         onMouseHover();
         break;
      case WM_MOUSELEAVE:
         if (_withHighlighting)
            onMouseLeave();
         break;
      default:
         break;
   }

   return ::DefSubclassProc(_handle, message, wParam, lParam);
}

int MultiTabControl :: addTabView(wstr_t title, void* param)
{
   int index = getTabCount();

   addTab(index, title, param, _iconId != 0 ? 0 : -1);

   return index;
}

void MultiTabControl :: renameTabView(int index, wstr_t title)
{
   // rename tab caption
   renameTab(index, title);
}

void MultiTabControl :: eraseTabView(int index)
{
   deleteTab(index);
}

void MultiTabControl :: refresh()
{
   _child->refresh();

   ControlBase::refresh();
}

// --- TabBar ---

TabBar :: TabBar(NotifierBase* notifier, bool withAbovescore, int height)
   : CustomTabBar(notifier, withAbovescore, false, 800, height),
   _current(nullptr), _pages(nullptr)
{
   _title = _T("Tabbar");

   _minHeight = 50;
   _minWidth = 50;
}

HWND TabBar :: createControl(HINSTANCE instance, ControlBase* owner)
{
   _handle = ::CreateWindowEx(
      TCS_EX_FLATSEPARATORS, WC_TABCONTROL, _title,
      WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER | TCS_FOCUSNEVER | TCS_TABS | TCS_SINGLELINE | TCS_OWNERDRAWFIXED,
      CW_USEDEFAULT, 0, _minWidth, _minHeight, owner->handle(), nullptr, instance, (LPVOID)this);

   return _handle;
}

void TabBar :: addTabChild(const wchar_t* name, ControlBase* child)
{
   auto rect = getClientRectangle();
   resizeTab(&rect, child);

   child->hide();

   _pages.add(child);

   addTab(_pages.count(), name, nullptr);
}

void TabBar :: removeTabChild(ControlBase* child)
{
   if (_pages.count() == 0)
      return;

   int index = _pages.retrieveIndex<ControlBase*>(child, [](ControlBase* arg, ControlBase* current)
      {
         return current == arg;
      });

   if (index != -1) {
      child->hide();
   }

   _pages.cut(child);
   deleteTab(index);

   _current = nullptr;

   refresh();
}

bool TabBar :: isChildAvailble(ControlBase* child)
{
   int index = _pages.retrieveIndex<ControlBase*>(child, [](ControlBase* arg, ControlBase* current)
      {
         return current == arg;
      });

   return index != -1;
}

bool TabBar :: selectTabChild(ControlBase* child)
{
   int index = _pages.retrieveIndex<ControlBase*>(child, [](ControlBase* arg, ControlBase* current)
      {
         return current == arg;
      });

   if (index != -1) {
      if (_current)
         _current->hide();

      _current = child;

      _current->show();

      selectTab(index);

      int x = getCurrentIndex();

      ControlBase::refresh();

      return true;
   }
   else return false;
}

void TabBar :: resizeTab(Rectangle* clientRect, ControlBase* control)
{
   Rectangle childRec(clientRect->topLeft.x + 4, clientRect->topLeft.y + 28, 
      clientRect->width() - 8, clientRect->height() - 36);

   control->setRectangle(childRec);
}

void TabBar :: setRectangle(Rectangle rec)
{
   ControlBase::setRectangle(rec);

   auto clientRect = getClientRectangle();
   for (auto it = _pages.start(); !it.eof(); ++it) {
      resizeTab(&clientRect, *it);
   }
}

void TabBar :: showCurrentTab()
{
   int index = getCurrentIndex();
   int current = 0;
   for (auto it = _pages.start(); !it.eof(); ++it) {
      if (index == current) {
         (*it)->show();
         (*it)->setFocus();
      }
      else (*it)->hide();

      current++;
   }
}

void TabBar :: refresh()
{
   if (_current)
      _current->refresh();

   CustomTabBar::refresh();
}

void TabBar::invalidate()
{
   if (_current)
      _current->invalidate();

   CustomTabBar::invalidate();
}
