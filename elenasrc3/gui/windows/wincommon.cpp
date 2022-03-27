//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Common Body File
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "wincommon.h"

using namespace elena_lang;

// --- ControlBase ---

HWND ControlBase :: create(HINSTANCE instance, wstr_t className, ControlBase* owner)
{
   _handle = ::CreateWindowW(className.str(), _title.str(), WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, owner ? owner->handle() : nullptr, nullptr, instance, this);

   return _handle;
}

void ControlBase :: showWindow(int cmdShow)
{
   ShowWindow(_handle, cmdShow);
   UpdateWindow(_handle);
}

elena_lang::Rectangle ControlBase :: getRectangle()
{
   RECT rc = { 0,0,0,0 };
   ::GetClientRect(_handle, &rc);

   return Rectangle(rc.left, rc.top, rc.right, rc.bottom);
}

void ControlBase :: setRectangle(Rectangle rec)
{
   ::MoveWindow(_handle, rec.topLeft.x, rec.topLeft.y, rec.width(), rec.height(), TRUE);
}

void ControlBase :: setFocus()
{
   ::SetFocus(_handle);
}

void ControlBase :: refresh()
{
   ::InvalidateRect(_handle, nullptr, false);
   ::UpdateWindow(_handle);
}

bool ControlBase :: visible()
{
   return ::IsWindowVisible(_handle) ? true : false;
}

// --- WindowBase ---

ATOM WindowBase :: registerClass(HINSTANCE hInstance, WNDPROC proc, wstr_t className, HICON icon, wstr_t menuName, HICON smallIcon, unsigned int style)
{
   WNDCLASSEXW wcex;

   wcex.cbSize = sizeof(WNDCLASSEX);

   wcex.style = style;
   wcex.lpfnWndProc = proc;
   wcex.cbClsExtra = 0;
   wcex.cbWndExtra = 0;
   wcex.hInstance = hInstance;
   wcex.hIcon = icon;
   wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
   wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
   wcex.lpszMenuName = menuName.str();
   wcex.lpszClassName = className.str();
   wcex.hIconSm = smallIcon;

   return RegisterClassExW(&wcex);
}

LRESULT WindowBase :: WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
   WindowBase* window = (WindowBase*)::GetWindowLongPtr(hWnd, GWLP_USERDATA);
   if (window == nullptr) {
      if (message == WM_CREATE) {
         window = (WindowBase*)((LPCREATESTRUCT)lParam)->lpCreateParams;
         ::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)window);

         window->assignHandle(hWnd);
      }
      else return ::DefWindowProc(hWnd, message, wParam, lParam);
   }

   return window->proceed(message, wParam, lParam);
}

void WindowBase :: setCursor(int type)
{
   HCURSOR cursor;

   switch (type) {
      case CURSOR_TEXT:
         cursor = ::LoadCursor(nullptr, IDC_IBEAM);
         break;
      case CURSOR_ARROW:
         cursor = ::LoadCursor(nullptr, IDC_ARROW);
         break;
      case CURSOR_SIZENS:
         cursor = ::LoadCursor(nullptr, IDC_SIZENS);
         break;
      case CURSOR_SIZEWE:
         cursor = ::LoadCursor(nullptr, IDC_SIZEWE);
         break;
      default:
         cursor = ::LoadCursor(nullptr, IDC_IBEAM);
   }
   ::SetCursor(cursor);
}

LRESULT WindowBase :: proceed(UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message) {
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
            if (onSetCursor())
               return TRUE;
         }
         break;
      //case WM_CLOSE:
      default:
         // to make compiler happy
         break;
   }
   return ::DefWindowProc(_handle, message, wParam, lParam);
}

// --- WindowApp ---

bool WindowApp :: initInstance(WindowBase* mainWindow)
{
   _hwnd = mainWindow->handle();

   if (!_hwnd)
   {
      return FALSE;
   }

   mainWindow->showWindow(_cmdShow);

   return TRUE;
}

void WindowApp :: notifyMessage(int messageCode)
{
   ExtNMHDR notification;

   notification.nmhrd.code = NMHDR_Message;
   notification.nmhrd.hwndFrom = _hwnd;
   notification.extParam = messageCode;

   ::SendMessage(_hwnd, WM_NOTIFY, 0, (LPARAM)&notification);
}

void WindowApp :: notifyModelChange(int modelCode)
{
   ExtNMHDR notification;

   notification.nmhrd.code = NMHDR_Model;
   notification.nmhrd.hwndFrom = _hwnd;
   notification.extParam = modelCode;

   ::SendMessage(_hwnd, WM_NOTIFY, 0, (LPARAM)&notification);
}

int WindowApp :: run(GUIControlBase* mainWindow)
{
   // Perform application initialization:
   if (!initInstance(dynamic_cast<WindowBase*>(mainWindow)))
   {
      return FALSE;
   }

   HACCEL hAccelTable = LoadAccelerators(_instance, _accelerators.str());

   MSG msg;

   // Main message loop:
   while (GetMessage(&msg, nullptr, 0, 0))
   {
      if (!TranslateAccelerator(_hwnd, hAccelTable, &msg))
      {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
   }

   return (int)msg.wParam;
}
