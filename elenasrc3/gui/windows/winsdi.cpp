//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI SDI Window body File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "winsdi.h"

using namespace elena_lang;

// --- LayoutManager ---

inline bool isVisible(ControlBase* control)
{
   return (control && control->visible());
}

void adjustVertical(int width, int& height, ControlBase* control)
{
   if (isVisible(control)) {
      elena_lang::Rectangle rect = control->getRectangle();

      rect.setWidth(width);
      if (height > rect.height() + 4) {
         height -= rect.height();
      }
      else {
         rect.setHeight(height - 4);
         height = 4;
      }

      control->setRectangle(rect);
   }
}

void adjustHorizontal(int& width, int height, ControlBase* control)
{
   if (isVisible(control)) {
      elena_lang::Rectangle rect = control->getRectangle();

      rect.setHeight(height);
      if (width > rect.width() + 4) {
         width -= rect.width();
      }
      else {
         rect.setWidth(width - 4);
         width = 4;
      }

      control->setRectangle(rect);
   }
}

void adjustClient(int width, int height, ControlBase* control)
{
   if (isVisible(control)) {
      elena_lang::Rectangle rect = control->getRectangle();

      rect.setWidth(width);
      rect.setHeight(height);

      control->setRectangle(rect);
   }
}

void LayoutManager :: resizeTo(Rectangle area)
{
   int totalHeight = area.height();
   int totalWidth = area.width();
   int y = area.topLeft.x;
   int x = area.topLeft.y;

   adjustVertical(totalWidth, totalHeight, _top);
   adjustVertical(totalWidth, totalHeight, _bottom);
   adjustHorizontal(totalWidth, totalHeight, _left);
   adjustHorizontal(totalWidth, totalHeight, _right);
   adjustClient(totalWidth, totalHeight, _center);

   if (isVisible(_top)) {
      Rectangle topRect = _top->getRectangle();

      _top->setRectangle({ area.topLeft.x, area.topLeft.y, 
         topRect.width(), topRect.height() });

      y += topRect.height();
   }
   if (isVisible(_bottom)) {
      Rectangle bottomRect = _bottom->getRectangle();

      _bottom->setRectangle({ area.topLeft.x, y + totalHeight,
         bottomRect.width(), bottomRect.height() });
   }
   if (isVisible(_left)) {
      Rectangle leftRect = _bottom->getRectangle();

      _left->setRectangle({ area.topLeft.x, y,
         leftRect.width(), leftRect.height() });

      x += leftRect.width();
   }
   if (isVisible(_right)) {
      Rectangle rightRect = _bottom->getRectangle();

      _left->setRectangle({ area.topLeft.x, y,
         rightRect.width(), rightRect.height() });
   }

   if (isVisible(_center))
      _center->setRectangle(area);
}

// --- SDIWindow ---

void SDIWindow :: registerSDIWindow(HINSTANCE hInstance, wstr_t className, HICON icon, wstr_t menuName, HICON smallIcon)
{
   WindowBase::registerClass(hInstance, WindowBase::WndProc, className, icon, menuName, smallIcon, CS_HREDRAW | CS_VREDRAW);
}

void SDIWindow :: setLayout(int center, int top, int bottom, int right, int left)
{
   if (top >= 0)
      _layoutManager.setTop(_children[top]);

   if (center >= 0)
      _layoutManager.setCenter(_children[center]);

   if (bottom >= 0)
      _layoutManager.setBottom(_children[bottom]);

   if (left >= 0)
      _layoutManager.setLeft(_children[left]);

   if (right >= 0)
      _layoutManager.setRight(_children[right]);
}

void SDIWindow :: onResize()
{
   Rectangle clientRect = getRectangle();

   _layoutManager.resizeTo(clientRect);
}

void SDIWindow :: onActivate()
{
}

LRESULT SDIWindow :: proceed(UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
      case WM_SIZE:
         if (wParam != SIZE_MINIMIZED) {
            onResize();
         }
         return 0;
         case WM_COMMAND:
         {
            if(!onCommand(LOWORD(wParam)))
               return DefWindowProc(_handle, message, wParam, lParam);
         }
         break;
      //case WM_PAINT:
      //{
      //   PAINTSTRUCT ps;
      //   HDC hdc = ::BeginPaint(_handle, &ps);
      //   
      //   drawControls(hdc);
      //   EndPaint(_handle, &ps);

      //   break;
      //}
      case WM_DESTROY:
         PostQuitMessage(0);
         break;
      case WM_ACTIVATE:
         if (LOWORD(wParam) != WA_INACTIVE) {
            onActivate();
         }
         return 0;
      case WM_DRAWITEM:
         onDrawItem((DRAWITEMSTRUCT*)lParam);
         return TRUE;
      default:
         return DefWindowProc(_handle, message, wParam, lParam);
   }
   return 0;

}

void SDIWindow :: onDrawItem(DRAWITEMSTRUCT* item)
{
   for (size_t i = 0; i < _childCounter; ++i) {
      if (_children[i]->checkHandle(item->hwndItem)) {
         _children[i]->onDrawItem(item);
      }
   }
}

//
//void SDIWindow :: drawControls(HDC& hdc)
//{
//   for (size_t i = 0; i < _controlLength; i++) {
//      (_controls[i])->draw(hdc);
//   }
//}

