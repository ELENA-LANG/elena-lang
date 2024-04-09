//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI SDI Window body File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "winsdi.h"

using namespace elena_lang;

// --- BoxBase ---

BoxBase :: BoxBase(bool stretchMode, int spacer)
   : _stretchMode(stretchMode), _spacer(spacer)
{

}

void BoxBase :: show()
{
   for (size_t i = 0; i < _list.count(); i++) {
      _list[i]->show();
   }
}

void BoxBase :: hide()
{
   for (size_t i = 0; i < _list.count(); i++) {
      _list[i]->hide();
   }
}

bool BoxBase :: visible()
{
   for (size_t i = 0; i < _list.count(); i++) {
      if (_list[i]->visible())
         return true;
   }

   return false;
}

void BoxBase :: setFocus()
{
   if (_list.count() > 0)
      _list[0]->setFocus();
}

void BoxBase :: refresh()
{
   for (size_t i = 0; i < _list.count(); i++) {
      _list[i]->refresh();
   }
}

// --- VerticalBox ---

VerticalBox :: VerticalBox(bool stretchMode, int spacer)
   : BoxBase(stretchMode, spacer)
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

   for (size_t i = 0; i < _list.count(); i++) {
      auto itemRect = _list[i]->getRectangle();

      if (i != 0)
         height += _spacer;

      height += itemRect.height();      

      if (width < itemRect.width())
         width = itemRect.width();
   }

   return elena_lang::Rectangle(x, y, width, height);
}

void VerticalBox :: setRectangle(Rectangle rec)
{
   int x = rec.topLeft.x;
   int y = rec.topLeft.y;
   int width = rec.width();
   int height = rec.height();

   int count = _list.count_int();
   if (_stretchMode) {
      int meanHeight = height / count;
      for (int i = 0; i < count; i++) {
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
      for (int i = 1; i < count; i++) {
         fixedHeight += _list[i]->getRectangle().height();
         fixedHeight += _spacer;
      }
      if (height > fixedHeight) {
         height -= fixedHeight;
      }
      else height = 5;
      for (int i = 0; i < count; i++) {
         if (i != 0)
            height = _list[i]->getRectangle().height();

         _list[i]->setRectangle({x, y, width, height});
         y += height;
         y += _spacer;
      }
   }
}

// --- HorizontalBox ---

HorizontalBox :: HorizontalBox(bool stretchMode, int spacer)
   : BoxBase(stretchMode, spacer)
{

}

void HorizontalBox :: append(GUIControlBase* item)
{
   if (_list.count() != 0) {
      auto itemRect = item->getRectangle();
      auto bottomRect = _list[_list.count() - 1]->getRectangle();

      item->setRectangle({ bottomRect.topLeft.x + _spacer + 1, bottomRect.bottomRight.y,
         bottomRect.width(), itemRect.height() });
   }

   _list.add(item);
}

elena_lang::Rectangle HorizontalBox :: getRectangle()
{
   int x = 0;
   int y = 0;
   int width = 0;
   int height = 0;

   auto topRect = _list[0]->getRectangle();
   x = topRect.topLeft.x;
   y = topRect.topLeft.y;

   for (size_t i = 0; i < _list.count(); i++) {
      auto itemRect = _list[i]->getRectangle();

      if (i != 0)
         width += _spacer;

      width += itemRect.width();

      if (height < itemRect.height())
         height = itemRect.height();
   }

   return elena_lang::Rectangle(x, y, width, height);
}

void HorizontalBox :: setRectangle(Rectangle rec)
{
   int x = rec.topLeft.x;
   int y = rec.topLeft.y;
   int width = rec.width();
   int height = rec.height();

   int count = _list.count_int();
   if (_stretchMode) {
      int meanWidth = width / count;
      for (int i = 0; i < count; i++) {
         if (i == count - 1)
            meanWidth = width;

         _list[i]->setRectangle({ x, y, meanWidth, height });
         width -= meanWidth;
         x += meanWidth;
         x += _spacer;
      }
   }
   else {
      int fixedWidth = 0;
      for (int i = 1; i < count; i++) {
         fixedWidth += _list[i]->getRectangle().width();
         fixedWidth += _spacer;
      }
      if (width > fixedWidth) {
         width -= fixedWidth;
      }
      else width = 5;
      for (int i = 0; i < count; i++) {
         if (i != 0)
            width = _list[i]->getRectangle().width();

         _list[i]->setRectangle({ x, y, width, height });
         x += width;
         x += _spacer;
      }
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
      Rectangle leftRect = _left->getRectangle();

      _left->setRectangle({ area.topLeft.x, y,
         leftRect.width(), leftRect.height() });

      x += leftRect.width();

      _left->refresh();
   }
   if (isVisible(_right)) {
      Rectangle rightRect = _right->getRectangle();

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

void SDIWindow :: onResizing(RECT* rect)
{
   if (rect->right - rect->left < _minWidth) {
      rect->right = rect->left + _minWidth;
   }
   if (rect->bottom - rect->top < _minHeight) {
      rect->bottom = rect->top + _minHeight;
   }
}

LRESULT SDIWindow :: proceed(UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message)
   {
      case WM_SIZING:
         onResizing((RECT*)lParam);
         return TRUE;
      case WM_COMMAND:
         if(!onCommand(LOWORD(wParam)))
            return DefWindowProc(_handle, message, wParam, lParam);
         return 0;
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
         return 0;
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
      case WM_GETMINMAXINFO:
      {
         MINMAXINFO* minMax = (MINMAXINFO*)lParam;

         minMax->ptMinTrackSize.y = 100;
         minMax->ptMinTrackSize.x = 500;

         return FALSE;
      }
      //case WM_CTLCOLORLISTBOX:
      //   if (_childBkBrush != nullptr) {
      //      return (LRESULT)_childBkBrush;
      //   }
      //   else return WindowBase::proceed(message, wParam, lParam);
      //   break;

      ////case WM_CTLCOLORDIALOG:
      //case WM_CTLCOLOREDIT:
      //{
      //   //SetBkMode((HDC)wParam, TRANSPARENT);
      //   //SetTextColor((HDC)wParam, RGB(0x2B, 0x4C, 0x67));

      //   //HBRUSH brush = CreateSolidBrush(RGB(0, 255, 0));
      //   //return (INT_PTR)brush;
      //   HBRUSH hBrushBackground = CreateSolidBrush(RGB(0, 0, 0));
      //   SetTextColor((HDC)wParam, RGB(0x2B, 0x4C, 0x67));
      //   SetBkColor((HDC)wParam, RGB(0, 255, 0));
      //   return (LRESULT)hBrushBackground;
      //}
      default:
         return WindowBase::proceed(message, wParam, lParam);
   }
   return 0;

}

void SDIWindow :: onDrawItem(DRAWITEMSTRUCT* item)
{
   for (size_t i = 0; i < _childCounter; ++i) {
      if (_children[i] && _children[i]->checkHandle(item->hwndItem)) {
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

bool SDIWindow :: onSetCursor()
{
   setCursor(CURSOR_ARROW);

   return true;
}


void SDIWindow::exit()
{
   ::SendMessage(_handle, WM_CLOSE, 0, 0);

}
