//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI SDI Window body File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "winsdi.h"

using namespace elena_lang;

// --- VerticalBox ---

VerticalBox :: VerticalBox(bool stretchMode, int spacer)
   : _stretchMode(stretchMode), _spacer(spacer)
{

}

void VerticalBox :: append(GUIControlBase* item)
{
   if (_list.count() != 0) {
      auto itemRect = item->getRectangle();
      auto bottomRect = _list[_list.count() - 1]->getRectangle();

      item->setRectangle({ bottomRect.topLeft.x, bottomRect.bottomRight.y + _spacer + 1, 
         bottomRect.width(), itemRect.height()});
   }

   _list.add(item);
}

elena_lang::Rectangle VerticalBox :: getRectangle()
{
   int x = 0;
   int y = 0;
   int width = 0;
   int height = 0;

   auto topRect = _list[0]->getRectangle();
   x = topRect.topLeft.x;
   y = topRect.topLeft.y;

   auto bottomRect = _list[_list.count() - 1]->getRectangle();

   width = bottomRect.bottomRight.x - x + 1;
   height = bottomRect.bottomRight.y - y + 1;

   return elena_lang::Rectangle(x, y, width, height);
}

void VerticalBox :: setRectangle(Rectangle rec)
{
   int x = rec.topLeft.x;
   int y = rec.topLeft.y;
   int width = rec.width();
   int height = rec.height();

   size_t count = _list.count();
   if (_stretchMode) {
      int meanHeight = height / count;
      for (size_t i = 0; i < count; i++) {
         if (i == count - 1)
            meanHeight = height;

         _list[i]->setRectangle({x, y, width, meanHeight });
         height -= meanHeight;
         y += meanHeight;
         y += _spacer;
      }
   }
   else {
      int fixedHeight = 0;
      for (size_t i = 1; i < count; i++) {
         fixedHeight += _list[i]->getRectangle().height();
         fixedHeight += _spacer;
      }
      if (height > fixedHeight) {
         height -= fixedHeight;
      }
      else height = 5;
      for (size_t i = 0; i < count; i++) {
         if (i != 0)
            height = _list[i]->getRectangle().height();

         _list[i]->setRectangle({x, y, width, height});
         y += height;
         y += _spacer;
      }
   }
}

void VerticalBox :: show()
{
   for (size_t i = 0; i < _list.count(); i++) {
      _list[i]->show();
   }
}

void VerticalBox :: hide()
{
   for (size_t i = 0; i < _list.count(); i++) {
      _list[i]->hide();
   }
}

bool VerticalBox :: visible()
{
   for (size_t i = 0; i < _list.count(); i++) {
      if (_list[i]->visible())
         return true;
   }

   return false;
}

void VerticalBox :: setFocus()
{
   if (_list.count() > 0)
      _list[0]->setFocus();
}

void VerticalBox :: refresh()
{
   for (size_t i = 0; i < _list.count(); i++) {
      _list[i]->refresh();
   }
}

// --- LayoutManager ---

inline bool isVisible(GUIControlBase* control)
{
   return (control && control->visible());
}

void adjustVertical(int width, int& height, GUIControlBase* control)
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

void adjustHorizontal(int& width, int height, GUIControlBase* control)
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

void adjustClient(int width, int height, GUIControlBase* control)
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

      _top->refresh();
   }
   if (isVisible(_bottom)) {
      Rectangle bottomRect = _bottom->getRectangle();

      _bottom->setRectangle({ area.topLeft.x, y + totalHeight,
         bottomRect.width(), bottomRect.height() });

      _bottom->refresh();
   }
   if (isVisible(_left)) {
      Rectangle leftRect = _bottom->getRectangle();

      _left->setRectangle({ area.topLeft.x, y,
         leftRect.width(), leftRect.height() });

      x += leftRect.width();

      _left->refresh();
   }
   if (isVisible(_right)) {
      Rectangle rightRect = _bottom->getRectangle();

      _right->setRectangle({ area.topLeft.x, y,
         rightRect.width(), rightRect.height() });

      _right->refresh();
   }

   if (isVisible(_center)) {
      _center->setRectangle({ x, y, totalWidth, totalHeight });

      _center->refresh();
   }      
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
   Rectangle clientRect = getClientRectangle();

   _layoutManager.resizeTo(clientRect);
}

void SDIWindow :: onActivate()
{
}

void SDIWindow :: onNotify(NMHDR* hdr)
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
         if(!onCommand(LOWORD(wParam)))
            return DefWindowProc(_handle, message, wParam, lParam);
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
      case WM_NOTIFY:
         onNotify((NMHDR*)lParam);
         return 0;
      default:
         return DefWindowProc(_handle, message, wParam, lParam);
   }
   return 0;

}

void SDIWindow :: onDrawItem(DRAWITEMSTRUCT* item)
{
   for (size_t i = 0; i < _childCounter; ++i) {
      if (_children[i]->checkHandle(item->hwndItem)) {
         ((ControlBase*)_children[i])->onDrawItem(item);
      }
   }
}

void SDIWindow :: close()
{
   ::SendMessage(_handle, WM_CLOSE, 0, 0);
}

//void SDIWindow :: drawControls(HDC& hdc)
//{
//   for (size_t i = 0; i < _controlLength; i++) {
//      (_controls[i])->draw(hdc);
//   }
//}

