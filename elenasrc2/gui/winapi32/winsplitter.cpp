//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Splitter class implementation
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "winsplitter.h"

using namespace _GUI_;

#ifndef WH_MOUSE_LL
#define WH_MOUSE_LL 14
#endif

static HWND	 hWndMouse = NULL;
static HHOOK hookMouse = NULL;

static LRESULT CALLBACK hookProcMouse(int nCode, WPARAM wParam, LPARAM lParam)
{
   if(nCode >= 0) {
      switch (wParam) {
         case WM_MOUSEMOVE:
         case WM_NCMOUSEMOVE:
            ::PostMessage(hWndMouse, (UINT)wParam, 0, 0);
            break;
         case WM_LBUTTONUP:
         case WM_NCLBUTTONUP:
            ::PostMessage(hWndMouse, (UINT)wParam, 0, 0);
            return TRUE;
         default:
            break;
      }
   }
   return ::CallNextHookEx(hookMouse, nCode, wParam, lParam);
}

// --- Splitter ---

Splitter :: Splitter(Control* owner, Control* client, bool vertical, int notifyCode)
   : Window(client->getLeft(), client->getTop(), 0, 0)
{
   _client = client;
   _vertical = vertical;
   _mouseCaptured = false;

   _minWidth = 4;
   _minHeight = 4;
   _cursor = _vertical ? CURSOR_SIZEWE : CURSOR_SIZENS;

   _notifyCode = notifyCode;

   _setWidth(_client->getWidth());
   _setHeight(_client->getHeight());

   _instance = owner->_getInstance();
   _owner = owner->getHandle();

   _handle = ::CreateWindowEx(
      0, _vertical ? VSPLTR_WND_CLASS : HSPLTR_WND_CLASS, _T("Splitter"), 
      WS_CHILD | WS_VISIBLE,
      _left, _top, _width, _height, _owner, NULL, _instance, (LPVOID)this);
}

void Splitter :: _registerClass(HINSTANCE instance, bool vertical)
{
   if (vertical) {
      Window::_registerClass(instance, VSPLTR_WND_CLASS, 
         CS_HREDRAW | CS_VREDRAW, ::LoadCursor(NULL,IDC_SIZEWE), (HBRUSH)COLOR_WINDOW, NULL, NULL);
   }
   else {
      Window::_registerClass(instance, HSPLTR_WND_CLASS, 
         CS_HREDRAW | CS_VREDRAW, ::LoadCursor(NULL,IDC_SIZENS), (HBRUSH)COLOR_WINDOW, NULL, NULL);
   }
}

bool Splitter :: _onSetCursor()
{
   _setCursor(_cursor);

   return true;
}

bool Splitter :: isVisible()
{
   if (_client->isVisible()!=Window::isVisible()) {
      if (_client->isVisible()) {
         show();
      }
      else hide();
   }
   return Window::isVisible();
}

int Splitter :: getWidth() const
{
   if (_vertical) {
      return _client->getWidth() + _width;
   }
   else return _height;
}

int Splitter :: getHeight() const
{
   if (!_vertical) {
      return _client->getHeight() + _height;
   }
   else return _width;
}

void Splitter :: _setCoordinate(int x, int y)
{   
   if (_vertical) {
      _client->_setCoordinate(x, y);
      _left = x + _client->getWidth();
	   _top = y;
   }
   else {
      _left = x;
	   _top = y;
	   _client->_setCoordinate(x, y + 3);
   }
}

void Splitter :: _setWidth(int width)
{
   if (_vertical) {
	  if (width > 3) {
         _client->_setWidth(width - 3);
	  }
	  else _client->_setWidth(1);

      _left = _client->getLeft() + _client->getWidth();
      _width = 3;
   }
   else {
      _client->_setWidth(width);
      Control::_setWidth(width);
   }
}

void Splitter :: _setHeight(int height)
{
   if (!_vertical) {
      if (height > 3) {
         _client->_setHeight(height - 3);
      }
      else _client->_setHeight(1);

      _height = 3;
   }
   else {
      _client->_setHeight(height);
      Control::_setHeight(height);
   }
}

void Splitter :: _resize()
{
   _client->_resize();

   Control::_resize();
}

LRESULT Splitter :: _WindProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch (Message)
   {
      case WM_LBUTTONDOWN:
         hWndMouse = hWnd;
         hookMouse = ::SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)hookProcMouse, _instance, 0);

         ::SetCapture(_handle);
         ::GetCursorPos(&_srcPos);
         _mouseCaptured = true;
         break;
      case WM_LBUTTONUP:
      case WM_NCLBUTTONUP:
         if (hookMouse) {
            ::UnhookWindowsHookEx(hookMouse);
            hookMouse = NULL;
         }
         ::SetCapture(NULL);
         _mouseCaptured = false;
         break;
      case WM_MOUSEMOVE:
      case WM_NCMOUSEMOVE:
         if (_mouseCaptured) {
            POINT	destPos;
            ::GetCursorPos(&destPos);

            if (!_vertical && (_srcPos.y != destPos.y)) {
               shiftOn(_srcPos.y - destPos.y);
            }
            else if (_srcPos.x != destPos.x) {
               shiftOn(destPos.x - _srcPos.x);
            }
            _srcPos = destPos;
         }
         break;
   }
   return Window::_WindProc(hWnd, Message, wParam, lParam);
}

void Splitter :: shiftOn(int delta)
{
   if (!_vertical) {
      if (getHeight() + delta > _minHeight) {
         _setHeight(getHeight() + delta);
      }
      else _setHeight(_minHeight);
   }
   else {	   
      if (getWidth() + delta > _minWidth) {
         _setWidth(getWidth() + delta);
      }
      else _setWidth(_minWidth);
   } 
   _notify(_owner, _notifyCode);
}
