//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Common Window Implementation
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "wincommon.h"

using namespace _GUI_;

// --- Clipboard ---

bool Clipboard :: isAvailable()
{
   return (::IsClipboardFormatAvailable(CF_UNICODETEXT)==TRUE);
}

bool Clipboard :: open(HWND id)
{
   return (::OpenClipboard(id)!=0);
}

HGLOBAL Clipboard :: create(size_t size)
{
   return ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, (size + 1) * 2);
}

wchar_t* Clipboard :: allocate(HGLOBAL buffer)
{
   return (wchar_t*)::GlobalLock(buffer);
}

void Clipboard :: copy(HGLOBAL buffer)
{
   ::SetClipboardData(CF_UNICODETEXT, buffer);
}

HGLOBAL Clipboard :: get()
{
   return ::GetClipboardData(CF_UNICODETEXT);
}

size_t Clipboard :: getSize(HGLOBAL buffer)
{
   return ::GlobalSize(buffer);
}

void Clipboard :: free(HGLOBAL buffer)
{
   ::GlobalUnlock(buffer);
}

void Clipboard :: clear()
{
   ::EmptyClipboard();
}

void Clipboard :: close()
{
   ::CloseClipboard();
}

// --- Control ---

Control :: Control(int left, int top, int width, int height)
{
   _minWidth = 0;
   _minHeight = 0;

   _left = left;
   _top = top;
   _width = width;
   _height = height;
}

void Control :: _setConstraint(int minWidth, int minHeight)
{
   _minWidth = minWidth;
   _minHeight = minHeight;
}

void Control :: show()
{
   ::ShowWindow(_handle, SW_SHOW);
}

void Control :: hide()
{
   ::ShowWindow(_handle, SW_HIDE);
}

bool Control :: isVisible()
{
   return ::IsWindowVisible(_handle) ? true : false;
}

_GUI_::Rectangle Control :: getRectangle()
{
   RECT rc={0,0,0,0};
   ::GetClientRect(_handle, &rc);

   return Rectangle(rc.left, rc.top, rc.right, rc.bottom);
}

void Control :: _setWidth(int width)
{
   if (width < _minWidth) {
      _width = _minWidth;
   }
   else _width = width;
}

void Control :: _setHeight(int height)
{
   if (height < _minHeight) {
      _height = _minHeight;
   }
   else _height = height;
}

void Control :: _setCoordinate(int x, int y)
{
   _left = x;
   _top = y;
}

void Control :: setFocus()
{
   ::SetFocus(_handle);
}

void Control :: _resize()
{
   ::MoveWindow(_handle, _left, _top, _width, _height, TRUE);

   refresh();
}

void Control :: refresh()
{
   ::InvalidateRect(_handle, NULL, false);
   ::UpdateWindow(_handle);
}

void Control :: _notify(HWND receptor, int code)
{
   NMHDR notification;

   notification.code = code;
   notification.hwndFrom = _handle;

   ::SendMessage(receptor, WM_NOTIFY, 0, (LPARAM)&notification);
}


void Control :: _notify(HWND receptor, int code, int extParam)
{
   ExtNMHDR notification;

   notification.nmhrd.code = code;
   notification.nmhrd.hwndFrom = _handle;
   notification.extParam = extParam;

   ::SendMessage(receptor, WM_NOTIFY, 0, (LPARAM)&notification);
}

// --- Window ---

void Window :: _registerClass(HINSTANCE hInstance, const wchar_t* name, UINT style, HCURSOR cursor, HBRUSH background, HICON icon, wchar_t* menu)
{
   WNDCLASSEX wndClass;
   wndClass.cbSize = sizeof(wndClass);
   wndClass.style = style;
   wndClass.lpfnWndProc = Window::_Proc;
   wndClass.cbClsExtra = 0;
   wndClass.cbWndExtra = 0;
   wndClass.hInstance = hInstance;
   wndClass.hIcon = icon;
   wndClass.hCursor = cursor;
   wndClass.hbrBackground = background;
   wndClass.lpszMenuName = menu;
   wndClass.lpszClassName = name;
   wndClass.hIconSm = 0;

   if (!::RegisterClassEx(&wndClass))	{
      throw 0; // !! todo: generate exception      
   }
}

LRESULT CALLBACK Window :: _Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   Window* window = (Window*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
   if (window==NULL) {
      if (Message==WM_CREATE) {
         window = (Window*)((LPCREATESTRUCT)lParam)->lpCreateParams;
         ::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);

         return window->_WindProc(hWnd, Message, wParam, lParam);
      }
      else return ::DefWindowProc(hWnd, Message, wParam, lParam);
   }
   else return window->_WindProc(hWnd, Message, wParam, lParam);
}

LRESULT Window :: _WindProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch (Message)
   {
      case WM_CLOSE:
         onClose();
         return 0;
      case WM_SIZE:
         if (wParam != SIZE_MINIMIZED) {
            onResize();
         }
         return 0;
      case WM_SETFOCUS:
         onSetFocus();
         return 0;
      case WM_KILLFOCUS:
         onLoseFocus();
         return 0;
      case WM_SETCURSOR:
         if (LOWORD(lParam) == HTCLIENT) {
            if (_onSetCursor())
               return TRUE;
         }
         break;
   }
   return ::DefWindowProc(hWnd, Message, wParam, lParam);
}

bool Window :: onClose()
{
   ::DestroyWindow(_handle);

   return true;
}

void Window :: _setCursor(int type)
{
   HCURSOR cursor;

   switch (type) {
      case CURSOR_TEXT:
         cursor = ::LoadCursor(NULL, IDC_IBEAM);
         break;
      case CURSOR_ARROW:
         cursor = ::LoadCursor(NULL, IDC_ARROW);
         break;
      case CURSOR_SIZENS:
         cursor = ::LoadCursor(NULL, IDC_SIZENS);
         break;
      case CURSOR_SIZEWE:
         cursor = ::LoadCursor(NULL, IDC_SIZEWE);
         break;
      default:
         cursor = ::LoadCursor(NULL, IDC_IBEAM);
   }
   ::SetCursor(cursor);
}

void Window :: setCaption(const wchar_t* caption)
{
   ::SetWindowText(_handle, caption);
}

// --- DateTime ---

DateTime DateTime :: getFileTime(const wchar_t* path)
{
   DateTime dt;

   HANDLE hFile = ::CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
   if (hFile) {
      FILETIME ftCreate, ftAccess, ftWrite;

      if (::GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite)) {
         FileTimeToSystemTime(&ftWrite, &dt._time);
      }
   }
   ::CloseHandle(hFile);

   return dt;
}
