//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI SDI Control Implementation File
//                                               (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "winsdi.h"

using namespace _GUI_;

// --- LayoutManager ---

bool isVisible(Control* control)
{
   return (control && control->isVisible());
}

void adjustVertical(int width, int& height, Control* control)
{
   if (isVisible(control)) {
      control->_setWidth(width);
      if (height > control->getHeight() + 4) {
         height -= control->getHeight();
      }
      else {
         control->_setHeight(height - 4);
         height = 4;
      }
   }
}

void adjustHorizontal(int& width, int height, Control* control)
{
   if (isVisible(control)) {
      control->_setHeight(height);
      if (width > control->getWidth() + 4) {
         width -= control->getWidth();
      }
      else {
         control->_setWidth(width - 4);
         width = 4;
      }
   }
}

void adjustClient(int width, int height, Control* control)
{
   if (isVisible(control)) {
      control->_setWidth(width);
      control->_setHeight(height);
   }
}

void LayoutManager :: resizeTo(_GUI_::Rectangle area)
{
   int totalHeight = area.Height();
   int totalWidth = area.Width();
   int y = area.topLeft.x;
   int x = area.topLeft.y;

   adjustVertical(totalWidth, totalHeight, _top);
   adjustVertical(totalWidth, totalHeight, _bottom);
   adjustHorizontal(totalWidth, totalHeight, _left);
   adjustHorizontal(totalWidth, totalHeight, _right);
   adjustClient(totalWidth, totalHeight, _client);

   if (isVisible(_top)) {
      _top->_setCoordinate(area.topLeft.x, area.topLeft.y);
      _top->_resize();
      y += _top->getHeight();
   }
   if (isVisible(_bottom)) {
      _bottom->_setCoordinate(area.topLeft.x, y + totalHeight);
      _bottom->_resize();
   }
   if (isVisible(_left)) {
      _left->_setCoordinate(area.topLeft.x, y);
      _left->_resize();
      x += _left->getWidth();
   }
   if (isVisible(_right)) {
      _right->_setCoordinate(area.topLeft.x, y);
      _right->_resize();
   }
   if (isVisible(_client)) {
      _client->_setCoordinate(x, y);
      _client->_resize();
   }
}

// --- SDIWindow ---

void SDIWindow :: _registerClass(HINSTANCE instance, HICON icon, wchar_t* menu)
{
   Window::_registerClass(instance, APP_WND_CLASS, CS_BYTEALIGNWINDOW | CS_DBLCLKS, NULL, (HBRUSH)COLOR_WINDOW, icon, menu);
}

LRESULT SDIWindow :: _WindProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch (Message)
   {
      case WM_SIZING:
        _onResizing((RECT*)lParam);
         return TRUE;
      case WM_COMMAND:
         //if (lParam == 0)
         _onMenuCommand(LOWORD(wParam));

         return 0;
      case WM_DESTROY:
         ::PostQuitMessage(0);
         return 0;
      case WM_DRAWITEM:
         _onDrawItem((DRAWITEMSTRUCT*)lParam);
         return TRUE;
      case WM_ACTIVATE:
         if (LOWORD(wParam) != WA_INACTIVE)  {
            onActivate();
         }
         return 0;
      case WM_NOTIFY:
         _onNotify((NMHDR*)lParam);
         return 0;
      case WM_GETMINMAXINFO:
      {
         MINMAXINFO *minMax = (MINMAXINFO*)lParam;

         minMax->ptMinTrackSize.y = 100;
         minMax->ptMinTrackSize.x = 500;

         return FALSE;
      }
   }
   return Window::_WindProc(hWnd, Message, wParam, lParam);
}

SDIWindow :: SDIWindow(HINSTANCE instance, const wchar_t* caption)
   : Window(0, 0, 200, 300) // !! temporal
{
   _statusBar = NULL;

   _instance = instance;
   _handle = ::CreateWindowEx(
      0, APP_WND_CLASS, caption, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
      _left, _top, _width, _height, NULL, NULL, _instance, (LPVOID)this);

   _setConstraint(_width, _height);
}

void SDIWindow :: _onResizing(RECT* rect)
{
   if (rect->right - rect->left < _minWidth) {
      rect->right = rect->left + _minWidth;
   }
   if (rect->bottom - rect->top < _minHeight) {
      rect->bottom = rect->top + _minHeight;
   }
}

void SDIWindow :: onResize()
{
   if (_statusBar) {
      Rectangle clientRect = getRectangle();
      clientRect.bottomRight.y -= _statusBar->getHeight();

      _layoutManager.resizeTo(clientRect);

      _statusBar->_setCoordinate(clientRect.topLeft.x, clientRect.topLeft.y + clientRect.bottomRight.y);
      _statusBar->_setWidth(clientRect.bottomRight.x);
      _statusBar->_resize();
   }
}

void SDIWindow :: onActivate()
{
}

void SDIWindow :: _onDrawItem(DRAWITEMSTRUCT* item)
{
}

void SDIWindow :: _onNotify(NMHDR* notification)
{
}

void SDIWindow :: show(bool maximized)
{
   ::ShowWindow(_handle, maximized ? SW_MAXIMIZE : SW_SHOW);
}

void SDIWindow :: exit()
{
   ::SendMessage(_handle, WM_CLOSE, 0, 0);
}

bool SDIWindow :: _onSetCursor()
{
   _setCursor(CURSOR_ARROW);

   return true;
}
