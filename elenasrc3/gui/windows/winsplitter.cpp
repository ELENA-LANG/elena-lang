//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      WinAPI Splitter class implementation
//                                              (C)2022, by Aleksex Rakov
//---------------------------------------------------------------------------

#include "winsplitter.h"

using namespace elena_lang;

static HWND	 hWndMouse = nullptr;
static HHOOK hookMouse = nullptr;

static LRESULT CALLBACK hookProcMouse(int nCode, WPARAM wParam, LPARAM lParam)
{
   if (nCode >= 0) {
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

Splitter :: Splitter(NotifierBase* notifier, int notifyCode, ControlBase* client, bool vertical)
   : WindowBase(nullptr, 800, 4),
   _instance(nullptr),
   _notifier(notifier),
   _notifyCode(notifyCode),
   _client(client),
   _vertical(vertical), 
   _srcPos({}),
   _mouseCaptured(false)
{
   _minWidth = 4;
   _minHeight = 4;

   _cursor = _vertical ? CURSOR_SIZEWE : CURSOR_SIZENS;
}

void Splitter :: registerSplitterWindow(HINSTANCE hInstance, wstr_t className, bool vertical)
{
   WindowBase::registerClass(hInstance, WindowBase::WndProc, className, nullptr, nullptr, nullptr, 
      vertical ? ::LoadCursor(NULL, IDC_SIZEWE) : ::LoadCursor(NULL, IDC_SIZENS),
      (HBRUSH)COLOR_WINDOW,
      CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS);
}

HWND Splitter :: create(HINSTANCE instance, wstr_t className, ControlBase* owner)
{
   _handle = ::CreateWindowEx(
      0, className, _title,
      WS_CHILD,
      CW_USEDEFAULT, 0, 4, 4, owner->handle(), nullptr, instance, (LPVOID)this);

   _instance = instance;

   return _handle;

}

bool Splitter :: visible()
{
   bool v = ControlBase::visible();

   if (_client->visible() != v) {
      if (_client->visible()) {
         show();

         v = true;
      }
      else {
         hide();
         v = false;
      }
   }
   return v;
}

elena_lang::Rectangle Splitter :: getRectangle()
{
   if (!visible()) {
      return {};
   }

   auto rec = _client->getRectangle();

   if (!_vertical) {
      return { _rect.topLeft.x, _rect.topLeft.y, rec.width(), rec.height() + _minHeight };
   }
   else return { _rect.topLeft.x, _rect.topLeft.y, rec.width() + _minWidth, rec.height() };
}

void Splitter :: setRectangle(elena_lang::Rectangle rec)
{
   if (!visible())
      return;

   int width = rec.width();
   int height = rec.height();

   if (!_vertical) {
      if (height > _minHeight) {
         height -= _minHeight;
      }
      else height = 1;

      _client->setRectangle({ rec.topLeft.x, rec.topLeft.y + _minHeight, width, height });
      ControlBase::setRectangle({ rec.topLeft.x, rec.topLeft.y, width, _minHeight });
   }
   else {
      if (width > _minWidth) {
         width -= _minWidth;
      }
      else width = 1;

      _client->setRectangle({ rec.topLeft.x, rec.topLeft.y, width, height });
      ControlBase::setRectangle({ rec.topLeft.x + _client->getRectangle().width(), rec.topLeft.y, _minWidth, height });
   }
}

void Splitter :: refresh()
{
   _client->refresh();

   ControlBase::refresh();
}

void Splitter :: onButtonDown(Point point, bool kbShift)
{
   hWndMouse = _handle;
   hookMouse = ::SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)hookProcMouse, _instance, 0);

   ::SetCapture(_handle);
   ::GetCursorPos(&_srcPos);
   _mouseCaptured = true;
}

void Splitter :: onButtonUp()
{
   if (hookMouse) {
      ::UnhookWindowsHookEx(hookMouse);
      hookMouse = nullptr;
   }
   ::SetCapture(nullptr);
   _mouseCaptured = false;
}

void Splitter :: onMove()
{
   if (_mouseCaptured) {
      POINT	destPos;
      ::GetCursorPos(&destPos);

      if (!_vertical && (_srcPos.y != destPos.y)) {
         shiftOn(_srcPos.y - destPos.y);
      }
      else if (_vertical && _srcPos.x != destPos.x) {
         shiftOn(destPos.x - _srcPos.x);
      }
      _srcPos = destPos;
   }
}

bool Splitter :: onSetCursor()
{
   setCursor(_cursor);

   return true;
}

LRESULT Splitter :: proceed(UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message) {
      case WM_LBUTTONDOWN:
         onButtonDown(Point(LOWORD(lParam), HIWORD(lParam)), (wParam & MK_SHIFT) != 0);
         break;
      case WM_LBUTTONUP:
      case WM_NCLBUTTONUP:
         onButtonUp();
         break;
      case WM_MOUSEMOVE:
      case WM_NCMOUSEMOVE:
         onMove();
         break;
      default:
         // to make compiler happy
         break;
   }

   return WindowBase::proceed(message, wParam, lParam);
}

void Splitter :: shiftOn(int delta)
{
   auto rec = getRectangle();

   int height = rec.height();
   int width = rec.width();

   if (!_vertical) {
      if (height + delta > _minHeight) {
         height += delta;
      }
      else height = _minHeight;
   }
   else {
      if (width + delta > _minWidth) {
         width += delta;
      }
      else width = _minWidth;
   }

   setRectangle({rec.topLeft.x, rec.topLeft.y, width, height});

   _notifier->notifyMessage(_notifyCode);
}
