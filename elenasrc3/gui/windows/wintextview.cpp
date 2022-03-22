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

// --- TextViewWindow ---

TextViewWindow :: TextViewWindow(TextViewModelBase* model, TextViewControllerBase* controller)
   : WindowBase(nullptr)
{
   _model = model;
   _controller = controller;
   _needToResize = _cached = false;
   _caretVisible = _caretValid = false;
   _mouseCaptured = false;
   _caret_x = 0;

   _model->docView->attachMotifier(this);
}

TextViewWindow :: ~TextViewWindow()
{
   _model->docView->removeNotifier(this);
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
      Style* marginStyle = _model->getStyle(STYLE_MARGIN);

      return marginStyle->avgCharWidth * 5;
   }
   else return 0;
}

void TextViewWindow :: resizeDocument()
{
   if (_model->isAssigned()) {
      Point     size;
      Rectangle client = getRectangle();
      auto style = _model->getStyle(STYLE_DEFAULT);

      int marginWidth = getLineNumberMargin();

      size.x = (client.width() - marginWidth) / style->avgCharWidth;
      size.y = client.height() / _model->getLineHeight();

      _model->resize(size);

      _needToResize = false;
   }
   _cached = false;

}

void TextViewWindow :: mouseToScreen(Point point, int& col, int& row, bool& margin)
{
   Rectangle rect = getRectangle();
   auto defaultStyle = _model->getStyle(STYLE_DEFAULT);
   int marginWidth = _model->getMarginWidth() + getLineNumberMargin();
   int offset = defaultStyle->avgCharWidth / 2;

   col = (point.x - rect.topLeft.x - marginWidth + offset) / defaultStyle->avgCharWidth;
   row = (point.y - rect.topLeft.y) / (_model->getLineHeight());
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

void TextViewWindow :: update(bool resize)
{
   refresh();
}

void TextViewWindow :: paint(Canvas& canvas, Rectangle clientRect)
{
   auto docView = _model->docView;

   Point caret = docView->getCaret(false) - docView->getFrame();

   Style* defaultStyle = _model->getStyle(STYLE_DEFAULT);
   Style* marginStyle = _model->getStyle(STYLE_MARGIN);
   int lineHeight = _model->getLineHeight();
   int marginWidth = _model->getMarginWidth() + getLineNumberMargin();

   if (!_cached) {
      
      if (!defaultStyle->valid) {
         _model->validate(&canvas);

         lineHeight = _model->getLineHeight();
         marginWidth = _model->getMarginWidth() + getLineNumberMargin();

         _needToResize = true;
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
         style = _model->getStyle(reader.style);
         length = writer.position();

         if (reader.newLine) {
            reader.newLine = false;

            x = clientRect.topLeft.x + marginWidth;
            y += lineHeight;
         }

         width = canvas.TextWidth(style, buffer, length);

         /*else */canvas.drawTextClipped(Rectangle(x, y, width + 1, lineHeight + 1), x, y,
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
            _caret_x += canvas.TextWidth(_model->getStyle(reader.style), buffer, writer.position());

            writer.reset();
         } while (reader.readCurrentLine(writer, 0xFF));

         docView->status.caretChanged = false;
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
      Rectangle   clientRect = getRectangle();

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
      createCaret(_model->getLineHeight(), /*_view->docView->status.overwriteMode ? _view->styles.getStyle(STYLE_DEFAULT)->avgCharWidth : */1);
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

   _model->docView->moveToFrame(col, row, kbShift);

   captureMouse();
}

void TextViewWindow :: onButtonUp()
{
   releaseMouse();
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
      default:
         return false;
   }

   return true;
}

LRESULT TextViewWindow :: proceed(UINT message, WPARAM wParam, LPARAM lParam)
{
   switch (message) {
      case WM_PAINT:
         onPaint();
         return 0;
      case WM_LBUTTONDOWN:
         onButtonDown(Point(LOWORD(lParam), HIWORD(lParam)), (wParam & MK_SHIFT) != 0);
         return 0;
      case WM_LBUTTONUP:
         onButtonUp();
         return 0;
      case WM_KEYDOWN:
         if (onKeyDown((int)wParam, isKeyDown(VK_SHIFT), isKeyDown(VK_CONTROL))) {
            return true;
         }
         else break;
      default:
         // to make compiler happy
         break;
   }

   return WindowBase::proceed(message, wParam, lParam);
}

void TextViewWindow :: onDocumentUpdate()
{
   if (_model->docView->status.isViewChanged()) {
      _cached = false;
      _caret_x = 0;
   }

   update();
}
