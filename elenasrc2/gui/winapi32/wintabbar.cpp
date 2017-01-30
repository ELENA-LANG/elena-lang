//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TabBar Implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "wintabbar.h"
#include "wingraphic.h"

using namespace _GUI_;

// --- TabBar ---

CustomTabBar :: CustomTabBar(Window* owner, bool withAbovescore)
   : Control(0, 0, 40, 40)
{
   _owner = owner->getHandle();
   _instance = owner->_getInstance();

   _withAbovescore = withAbovescore;
   _notSelected = true;
}

void CustomTabBar :: _onDrawItem(DRAWITEMSTRUCT* item)
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

         canvas.fillRectangle(barRect, Colour(255, 190, 128));
      }
   }
   else {
      canvas.fillRectangle(rect, Colour(220, 220, 220));
   }   

   canvas.setTransparentMode(true);

   if (isSelected) {
      //rect.topLeft.y -= ::GetSystemMetrics(SM_CYEDGE);
      rect.topLeft.y += 1;

      canvas.drawText(rect, label, (int)_ELENA_::getlength(label), Colour(0, 0, 0), true);
   } 
   else canvas.drawText(rect, label, (int)_ELENA_::getlength(label), Colour(128, 128, 128), true);
}

int CustomTabBar :: getCurrentIndex()
{
   return (int)::SendMessage(_handle, TCM_GETCURSEL, 0, 0);
}

int CustomTabBar :: getTabCount()
{
   return (int)::SendMessage(_handle, TCM_GETITEMCOUNT, 0, 0);
}

void* CustomTabBar :: getTabParam(int index)
{
   TCITEM tie;
   tie.mask = TCIF_PARAM;
   tie.iImage = -1;

   ::SendMessage(_handle, TCM_GETITEM, index, (LPARAM)&tie);

   return (void*)tie.lParam;
}

void CustomTabBar :: addTab(int index, const wchar_t* name, void* param)
{
   TCITEM tie;
   tie.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
   tie.iImage = -1;
   tie.pszText = (wchar_t*)name;
   tie.lParam = (LPARAM)param;

   ::SendMessage(_handle, TCM_INSERTITEM, index, (LPARAM)&tie);
}

void CustomTabBar :: selectTab(int index)
{
   int previous = (int)::SendMessage(_handle, TCM_SETCURSEL, index, 0);
   if (_notSelected || previous != index) {
      _notify(_owner, TCN_SELCHANGE);

      _notSelected = false;
   }
}

void CustomTabBar :: getTabName(int index, wchar_t* name, int length)
{
   TCITEM tie;
   tie.mask = TCIF_TEXT;
   tie.pszText = name;
   tie.cchTextMax = length - 1;

   ::SendMessage(_handle, TCM_GETITEM, index, (LPARAM)&tie);
}

void CustomTabBar :: deleteTab(int index)
{
   ::SendMessage(_handle, TCM_DELETEITEM, index, 0);  
}

void CustomTabBar :: renameTab(int index, const wchar_t* newName)
{
   // rename tab caption
   TCITEM tie;
   tie.mask = TCIF_TEXT | TCIF_IMAGE;
   tie.iImage = -1;
   tie.pszText = (wchar_t*)newName;

   ::SendMessage(_handle, TCM_SETITEM, index, (LPARAM)&tie);
}

// --- MultiTabView ---

MultiTabView :: MultiTabView(Window* owner, bool withAbovescore, Control* child)
   : CustomTabBar(owner, withAbovescore)
{
   _handle = ::CreateWindowEx(
      TCS_EX_FLATSEPARATORS, WC_TABCONTROL, _T("Tabbar"), 
      WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER | TCS_FOCUSNEVER | TCS_TABS | TCS_SINGLELINE | TCS_OWNERDRAWFIXED | TCS_TOOLTIPS,
      _left, _top, _width, _height, owner->getHandle(), NULL, _instance, (LPVOID)this);

   _child = child;
   if (_child) {
      _child->_setCoordinate(4, 28);
   }   
}

void MultiTabView :: eraseTabView(int index)
{
   deleteTab(index);

   // select next
   int count = getTabCount();
   if(count > 0) {
      if (index >= count) {
         index = count - 1;
      }
      selectTab(index);
   }
   else _notify(_owner, TCN_SELCHANGE);
}

int MultiTabView :: addTabView(const wchar_t* name, void* param)
{
   int index = getTabCount();

   addTab(index, name, param);

   selectTab(index);  // !! temporal, probably we should open the tab next to the current one

   return index;
}

void MultiTabView :: renameTabView(int index, const wchar_t* newName)
{
   // rename tab caption
   renameTab(index, newName);
}

void MultiTabView :: _setWidth(int width)
{   
   Control::_setWidth(width);
   if (_child) {
      _child->_setWidth(width - 8);
   }
}

void MultiTabView :: _setHeight(int height)
{ 
   Control::_setHeight(height);
   if (_child) {
      _child->_setHeight(height - 32);
   }
}


void MultiTabView :: onSetFocus()
{
   if (_child) {
      _child->setFocus();
   }
}

void MultiTabView :: _resize()
{
   Control::_resize();

   if (_child)
      _child->_resize();	
}

void MultiTabView :: setFocus()
{
   if (_child->isVisible()) {
      _child->setFocus();
   }
}

// --- TabBar ---

TabBar :: TabBar(Window* owner, bool withAbovescore)
   : CustomTabBar(owner, withAbovescore)
{
   _handle = ::CreateWindowEx(
      TCS_EX_FLATSEPARATORS, WC_TABCONTROL, _T("Tabbar"), 
      WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_BORDER | TCS_FOCUSNEVER | TCS_TABS | TCS_SINGLELINE | TCS_OWNERDRAWFIXED,
      _left, _top, _width, _height, owner->getHandle(), NULL, _instance, (LPVOID)this);

   _child = NULL;
}

void TabBar :: addTabChild(const wchar_t* name, Control* child)
{
   child->_setCoordinate(4, 28);
   child->_setWidth(_width - 14);
   child->_setHeight(_height - 36);
   child->hide();

   _children.add(child);

   addTab(_children.Count(), name, NULL);
}

void TabBar :: removeTabChild(Control* window)
{
   if (_children.Count() == 0)
      return;

   int index = 0;
   _ELENA_::List<Control*>::Iterator it = _children.start();
   while (!it.Eof() && ((*it) != window)) {
      index++;
      it++;
   }

   if (_child) {
      _child->hide();
      _child = NULL;
   }

   _children.cut(window);
   deleteTab(index);

   refresh();
}

void TabBar :: selectTabChild(Control* window)
{
   int index = 0;
   _ELENA_::List<Control*>::Iterator it = _children.start();
   while (!it.Eof()) {
      if (*it == window) {
         selectTabChild(index);

         break;
      }
      index++;
      it++;
   }
}

void TabBar :: selectTabChild(int index)
{
   if (_child)
      _child->hide();

   _child = *_children.get(index);
   if (_child) {
      _child->show();
   }
   selectTab(index);

   Control::refresh();
}

void TabBar :: _setWidth(int width)
{   
   Control::_setWidth(width);
   _ELENA_::List<Control*>::Iterator it = _children.start();
   while (!it.Eof()) {
	  (*it)->_setWidth(width - 14);
      it++;
   }   
}

void TabBar :: _setHeight(int height)
{ 
   Control::_setHeight(height);
   _ELENA_::List<Control*>::Iterator it = _children.start();
   while (!it.Eof()) {
	  (*it)->_setHeight(height - 36);
      it++;
   }   
}

void TabBar :: _resize()
{
   Control::_resize();
   _ELENA_::List<Control*>::Iterator it = _children.start();
   while (!it.Eof()) {
      (*it)->_resize();
      it++;
   }   
}

void TabBar :: refresh()
{
   int index = getCurrentIndex();
   if (index >= 0) {
      Control* current = *_children.get(index);
      if (_child != current) {
         if (_child)
            _child->hide();

         current->show();
         current->setFocus();

         _child = current;
      }
   }

   Control::refresh();
}
