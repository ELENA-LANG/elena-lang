//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TextView Control Body File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "wintextview.h"

using namespace elena_lang;

inline bool isKeyDown(int key)
{
   return (::GetKeyState(key) & 0x80000000) != 0;
}

// --- ViewStyles ---

void ViewStyles::assign(pos_t count, StyleInfo* styles, int lineHeight, int marginWidth, FontFactory* factory)
{
   this->count = count;
   this->items = new Style[count];
   this->marginWidth = marginWidth;
   this->lineHeight = lineHeight;

   for (size_t i = 0; i < count; i++) {
      Font* font = factory->createFont(styles[i].faceName, styles[i].size, styles[i].characterSet,
         styles[i].bold, styles[i].italic);

      this->items[i] = { styles[i].foreground, styles[i].background, font };
   }
}

void ViewStyles::release()
{
   if (count > 0)
      delete[] items;

   count = 0;
}

// --- TextViewWindow ---

TextViewWindow :: TextViewWindow(TextViewModelBase* model, TextViewControllerBase* controller, ViewStyles* styles)
   : WindowBase(nullptr, 50, 50)
{
   _model = model;
   _styles = styles;
   _controller = controller;
   _needToResize = _cached = false;
   _caretVisible = _caretValid = false;
   _mouseCaptured = false;
   _caret_x = 0;

   _model->attachDocListener(this);
}

TextViewWindow :: ~TextViewWindow()
{
   _model->removeDocListener(this);
}

void TextViewWindow :: registerTextViewWindow(HINSTANCE hInstance, wstr_t className)
{
   WindowBase::registerClass(hInstance, WindowBase::WndProc, className, nullptr, nullptr, nullptr, CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS);
}

HWND TextViewWindow :: create(HINSTANCE instance, wstr_t className, ControlBase* owner)
{
   _handle = ::CreateWindowEx(
      WS_EX_CLIENTEDGE, className, _title,
      WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN | WS_EX_RTLREADING,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, owner->handle(), nullptr, instance, (LPVOID)this);

   return _handle;
}

int TextViewWindow :: getLineNumberMargin()
{
   if (_model->lineNumbersVisible) {
      Style* marginStyle = _styles->getStyle(STYLE_MARGIN);

      return marginStyle->avgCharWidth * 5;
   }
   else return 0;
}

bool TextViewWindow :: getScrollInfo(int bar, SCROLLINFO* info)
{
   memset(info, 0, sizeof(*info));
   info->cbSize = sizeof(*info);
   info->fMask = SIF_ALL;

   return ::GetScrollInfo(_handle, bar, info) ? true : false;
}

void TextViewWindow :: resizeDocument()
{
   if (_model->isAssigned()) {
      Point     size;
      Rectangle client = getRectangle();
      auto style = _styles->getStyle(STYLE_DEFAULT);

      int marginWidth = getLineNumberMargin();

      size.x = (client.width() - marginWidth) / style->avgCharWidth;
      size.y = client.height() / _styles->getLineHeight();

      _needToResize = false;
      _model->resize(size);
   }
   _cached = false;
}

void TextViewWindow :: mouseToScreen(Point point, int& col, int& row, bool& margin)
{
   Rectangle rect = getClientRectangle();
   auto defaultStyle = _styles->getStyle(STYLE_DEFAULT);
   int marginWidth = _styles->getMarginWidth() + getLineNumberMargin();
   int offset = defaultStyle->avgCharWidth / 2;

   col = (point.x - rect.topLeft.x - marginWidth + offset) / defaultStyle->avgCharWidth;
   row = (point.y - rect.topLeft.y) / (_styles->getLineHeight());
   margin = (point.x - rect.topLeft.x < marginWidth);
}

void TextViewWindow :: createCaret(int height, int width)
{
   _caretValid = true;
   ::CreateCaret(_handle, nullptr, width, height);
}

void TextViewWindow :: destroyCaret()
{
   ::DestroyCaret();
   _caretValid = false;
}

void TextViewWindow :: showCaret()
{
   ::ShowCaret(_handle);
}

void TextViewWindow :: hideCaret()
{
   ::HideCaret(_handle);
}

void TextViewWindow :: locateCaret(int x, int y)
{
   ::SetCaretPos(x, y);
}

void TextViewWindow :: captureMouse()
{
   _mouseCaptured = true;
   ::SetCapture(_handle);
}

void TextViewWindow :: releaseMouse()
{
   _mouseCaptured = false;
   ::ReleaseCapture();
}

int TextViewWindow :: getHScrollerPosition()
{
   return _model->DocView()->getFrame().x;
}

int TextViewWindow :: getVScrollerPosition()
{
   return _model->DocView()->getFrame().y;
}

void TextViewWindow :: update(bool resized)
{
   if (_model->isAssigned()) {
      auto docView = _model->DocView();

      updateVScroller(resized);
      if (docView->status.maxColChanged) {
         updateHScroller(resized);
         docView->status.maxColChanged = false;
      }
      else updateHScroller(resized);
   }

   ControlBase::refresh();
}

void TextViewWindow :: updateHScroller(bool resized)
{
   int position = getHScrollerPosition();
   if (resized) {
      auto docView = _model->DocView();

      Point size = _model->DocView()->getSize();
      int max = docView->getMaxColumn();
      setScrollInfo(SB_HORZ, max - 1, size.x);
   }
   setScrollPosition(SB_HORZ, position);
}

void TextViewWindow :: updateVScroller(bool resized)
{
   int position = getVScrollerPosition();
   if (resized)  {
      auto docView = _model->DocView();

      Point size = _model->DocView()->getSize();
      int max = docView->getRowCount();
      setScrollInfo(SB_VERT, max + size.y, size.y);
   }
   setScrollPosition(SB_VERT, position);
}

void TextViewWindow :: setScrollInfo(int bar, int max, int page)
{
   SCROLLINFO info;

   info.cbSize = sizeof(info);
   info.fMask = SIF_PAGE | SIF_RANGE;
   info.nMin = 0;
   info.nMax = max;
   info.nPage = page;
   info.nPos = 0;
   info.nTrackPos = 1;

   ::SetScrollInfo(_handle, bar, &info, TRUE);
}

void TextViewWindow :: setScrollPosition(int bar, int position)
{
   ::SetScrollPos(_handle, bar, position, TRUE);
}

void TextViewWindow :: paint(Canvas& canvas, Rectangle clientRect)
{
   auto docView = _model->DocView();
   if (!docView)
      return;

   Point caret = docView->getCaret(false) - docView->getFrame();

   Style* defaultStyle = _styles->getStyle(STYLE_DEFAULT);
   Style* marginStyle = _styles->getStyle(STYLE_MARGIN);
   int lineHeight = _styles->getLineHeight();
   int marginWidth = _styles->getMarginWidth() + getLineNumberMargin();

   if (!_cached) {
      if (!defaultStyle->valid) {
         _styles->validate(&canvas);

         lineHeight = _styles->getLineHeight();
         marginWidth = _styles->getMarginWidth() + getLineNumberMargin();

         _needToResize = true;
         if (_caretValid) {
            createCaret(_styles->getLineHeight(), /*_view->docView->status.overwriteMode ? _view->styles.getStyle(STYLE_DEFAULT)->avgCharWidth : */1);
         }
      }
      // if document size yet to be defined
      if (_needToResize)
         resizeDocument();

      // Draw background
      clientRect.bottomRight += Point(1, 1);
      canvas.fillRectangle(clientRect, defaultStyle);

      // Draw margin
      Rectangle margin(clientRect.topLeft.x, clientRect.topLeft.y,
         marginWidth, clientRect.height());

      canvas.fillRectangle(margin, marginStyle);

      // Draw text
      canvas.setClipArea(clientRect);

      clientRect.bottomRight -= Point(1, 1);

      int x = clientRect.topLeft.x + marginWidth;
      int y = clientRect.topLeft.y - lineHeight + 1;
      int width = 0;

      Style* style = defaultStyle;
      text_c buffer[255];
      StringTextWriter<text_c, 255> writer(buffer);
      pos_t length = 0;
      
      DocumentView::LexicalReader reader(docView);
      reader.readFirst(writer, 255);
      do {
         style = _styles->getStyle(reader.style);
         length = writer.position();

         if (reader.newLine) {
            reader.newLine = false;

            x = clientRect.topLeft.x + marginWidth;
            y += lineHeight;
         }
         if (reader.bandStyle) {
            canvas.fillRectangle(Rectangle(x, y, clientRect.bottomRight.x - x, lineHeight + 1), style);

            reader.bandStyle = false;
         }

         width = canvas.TextWidth(style, buffer, length);

         if (length == 0 && reader.style == STYLE_SELECTION) {
            canvas.drawTextClipped(Rectangle(x, y, style->avgCharWidth + 1, lineHeight + 1), x, y,
               _T(" "), 1, style);
         }
         else canvas.drawTextClipped(Rectangle(x, y, width + 1, lineHeight + 1), x, y,
            buffer, length, style);

         x += width;
         writer.reset();
      } while (reader.readNext(writer, 255));

      _cached = true;
   }

   if (caret.x >= 0 && caret.y >= 0) {
      if (_caret_x == 0 || docView->status.caretChanged) {
         _caret_x = 0;

         text_c buffer[255];
         StringTextWriter<text_c, 255> writer(buffer);

         DocumentView::LexicalReader reader(docView);
         reader.seekCurrentLine();
         reader.readCurrentLine(writer, 255);
         do {
            _caret_x += canvas.TextWidth(_styles->getStyle(reader.style), buffer, writer.position());

            writer.reset();
         } while (reader.readCurrentLine(writer, 0xFF));
      }

      locateCaret(clientRect.topLeft.x + marginWidth + _caret_x,
         lineHeight * caret.y + 1);

      _caretVisible = true;
   }
   else _caretVisible = false;
}

void TextViewWindow :: onPaint()
{
   if (_model) {
      if (_caretValid && _caretVisible)
         hideCaret();

      PAINTSTRUCT ps;
      Rectangle   clientRect = getClientRectangle();

      ::BeginPaint(_handle, &ps);

      Canvas canvas(ps.hdc);

      if (_zbuffer.isReleased()) {
         _cached = false;
         _zbuffer.clone(&canvas, clientRect.width(), clientRect.height());
      }

      paint(_zbuffer, clientRect);
      canvas.copy(clientRect, Point(0, 0), _zbuffer);

      ::EndPaint(_handle, &ps);

      if (_caretValid && _caretVisible)
         showCaret();
   }
}

void TextViewWindow :: onResize()
{
   WindowBase::onResize();

   _zbuffer.release();

   resizeDocument();
   update(true);
}

void TextViewWindow :: onSetFocus()
{
   if (_model) {
      _caretVisible = true;
      createCaret(_styles->getLineHeight(), /*_view->docView->status.overwriteMode ? _view->styles.getStyle(STYLE_DEFAULT)->avgCharWidth : */1);
      showCaret();
      refresh();
   }
}

void TextViewWindow :: onLoseFocus()
{
   destroyCaret();
   _caretVisible = _caretValid = false;
}

bool TextViewWindow :: onSetCursor()
{
   POINT pt;
   ::GetCursorPos(&pt);
   ::ScreenToClient(_handle, &pt);

   int col, row;
   bool margin = false;
   mouseToScreen(Point(pt.x, pt.y), col, row, margin);

   if (margin) {
      setCursor(CURSOR_ARROW);
   }
   else setCursor(CURSOR_TEXT);

   return true;
}

void TextViewWindow :: onButtonDown(Point point, bool kbShift)
{
   setFocus();

   int col = 0, row = 0;
   bool margin = false;
   mouseToScreen(point, col, row, margin);

   _model->DocView()->moveToFrame(col, row, kbShift);

   captureMouse();
}

void TextViewWindow :: onButtonUp()
{
   releaseMouse();
}

void TextViewWindow :: onMouseMove(short wheelDelta, bool kbCtrl)
{
   auto docView = _model->DocView();

   int offset = (wheelDelta > 0) ? -1 : 1;
   if (kbCtrl) {
      offset *= docView->getSize().y;
   }
   docView->vscroll(offset);

   onDocumentUpdate();
}

bool TextViewWindow :: onKeyDown(int keyCode, bool kbShift, bool kbCtrl)
{
   switch (keyCode) {
      case VK_LEFT:
         _controller->moveCaretLeft(_model, kbShift, kbCtrl);
         break;
      case VK_RIGHT:
         _controller->moveCaretRight(_model, kbShift, kbCtrl);
         break;
      case VK_UP:
         _controller->moveCaretUp(_model, kbShift, kbCtrl);
         break;
      case VK_DOWN:
         _controller->moveCaretDown(_model, kbShift, kbCtrl);
         break;
      case VK_HOME:
         _controller->moveCaretHome(_model, kbShift, kbCtrl);
         break;
      case VK_END:
         _controller->moveCaretEnd(_model, kbShift, kbCtrl);
         break;
      case VK_DELETE:
      {
         auto docView = _model->DocView();
         if (docView->status.readOnly)
            return false;

         docView->eraseChar(false);
         onDocumentUpdate();
         break;
      }
      default:
         return false;
   }

   return true;
}

bool TextViewWindow :: onKeyPressed(wchar_t ch)
{
   if (_model->isAssigned()) {
      switch (ch) {
         case 0x0D:
            if (!_controller->insertNewLine(_model))
               return false;
            break;
         case 0x08:
            if(!_controller->eraseChar(_model, true))
               return false;
            break;
         case 0x09:
            _controller->indent(_model);
            break;
         default:
            if (!_controller->insertChar(_model, ch))
               return false;
      }

      onDocumentUpdate();
      return true;
   }
   else return false;
}

LRESULT TextViewWindow :: proceed(UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message) {
      case WM_PAINT:
         onPaint();
         return 0;
      case WM_CHAR:
         if (onKeyPressed((wchar_t)wParam)) {
            return 0;
         }
         else break;
      case WM_LBUTTONDOWN:
         onButtonDown(Point(LOWORD(lParam), HIWORD(lParam)), (wParam & MK_SHIFT) != 0);
         return 0;
      case WM_LBUTTONUP:
         onButtonUp();
         return 0;
      case WM_MOUSEWHEEL:
         onMouseMove(HIWORD(wParam), (wParam & MK_CONTROL) != 0);
         return 0;
      case WM_KEYDOWN:
         if (onKeyDown((int)wParam, isKeyDown(VK_SHIFT), isKeyDown(VK_CONTROL))) {
            return true;
         }
         else break;
      case WM_VSCROLL:
         onScroll(SB_VERT, LOWORD(wParam));
         return 0;
      case WM_HSCROLL:
         onScroll(SB_HORZ, LOWORD(wParam));
         return 0;
      default:
         // to make compiler happy
         break;
   }

   return WindowBase::proceed(message, wParam, lParam);
}

void TextViewWindow :: onDocumentUpdate()
{
   auto docView = _model->DocView();

   if (docView && docView->status.isViewChanged()) {
      _cached = false;
      _caret_x = 0;
   }

   update();
}

void TextViewWindow :: onScroll(int bar, int type)
{
   SCROLLINFO info;
   getScrollInfo(bar, &info);
   int offset = 0;
   switch (type) {
      case SB_LINEUP:
         if (info.nPos <= info.nMin)
            return;

         offset = -1;
         break;
      case SB_LINEDOWN:
         if (info.nPos >= info.nMax)
            return;

         offset = 1;
         break;
      case SB_THUMBPOSITION:
      case SB_THUMBTRACK:
         offset = info.nTrackPos - getScrollerPosition(bar);
         break;
      case SB_PAGEDOWN:
         offset = info.nPage;
         break;
      case SB_PAGEUP:
         offset = -(int)info.nPage;
         break;
      default:
         return;
   }

   auto docView = _model->DocView();
   if (bar == SB_VERT) {
      docView->vscroll(offset);
   }
   else docView->hscroll(offset);

   onDocumentUpdate();

}

void TextViewWindow :: refresh()
{
   onDocumentUpdate();
}
