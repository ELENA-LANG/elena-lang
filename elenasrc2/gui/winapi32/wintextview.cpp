//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TextView Control Implementation File
//                                               (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#include "wintextview.h"

using namespace _GUI_;

inline bool isKeyDown(int key)
{
   return (::GetKeyState(key) & 0x80000000) != 0;
}

// --- ViewStyles ---

void ViewStyles :: assign(size_t count, StyleInfo* styles, int lineHeight, int marginWidth)
{
   _count = count;
   _styles = new Style[count];
   _lineHeight = lineHeight;
   _marginWidth = marginWidth;

   for (size_t i = 0 ; i < count ; i++) {
      Font* font = Font::createFont(styles[i].faceName, styles[i].characterSet, styles[i].size,
         styles[i].bold, styles[i].italic);

      _styles[i] = Style(styles[i].foreground, styles[i].background, font);
   }
}

void ViewStyles :: validate(Canvas* canvas)
{
   for (size_t i = 0 ; i < _count ; i++) {
      _styles[i].validate(canvas);

      if (_styles[i].valid && _lineHeight < _styles[i].lineHeight) {
         _lineHeight = _styles[i].lineHeight;
      }
   }
}

void ViewStyles :: release()
{
   if (_count > 0)
      delete[] _styles;

   _count = 0;
}

// --- TextView ---

TextView :: TextView(Control* owner, int left, int top, int width, int height)
   : Window(left, top, width, height)
{
   _receptor = owner;
   _document = NULL;
   _instance = owner->_getInstance();
   _needToResize = true;
   _tabUsing = false;
   _tabSize = 4;

   _cached = false;
   _caretVisible = _caretValid = false;
   _mouseCaptured = false;
   _lineNumbersVisible = true;
   _highlight = false;

   _caret_x = 0;

   _handle = ::CreateWindowEx(
      WS_EX_CLIENTEDGE, EDIT_WND_CLASS, _T("TextView"), 
      WS_CHILD | WS_VSCROLL | WS_HSCROLL | WS_CLIPCHILDREN | WS_EX_RTLREADING,
      _left, _top, _width, _height, owner->getHandle(), NULL, _instance, (LPVOID)this);
}

void TextView :: applySettings(int tabSize, bool tabUsing, bool lineNumberVisible, bool highlight)
{
   _needToResize = (_lineNumbersVisible != lineNumberVisible);
   bool tabChanged = (_tabSize != tabSize);
   _cached &= !(_needToResize || _tabSize != tabSize || _highlight != highlight);

   _tabUsing = tabUsing;
   _tabSize = tabSize;
   _lineNumbersVisible = lineNumberVisible;
   _highlight = highlight;

   if (_document) {
      _document->setHighlightMode(_highlight);

      refreshView();
   }
}

void TextView :: setStyles(int count, StyleInfo* styles, int lineHeight, int marginWidth)
{
   _styles.release();
   _styles.assign(count, styles, lineHeight, marginWidth);

   _cached = false;
}

void TextView :: _registerClass(HINSTANCE instance)
{
   Window::_registerClass(instance, EDIT_WND_CLASS, CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS);
}

void TextView :: update(bool resized)
{
   if (_document) {
      updateVScroller(resized);
      if (_document->status.maxColChanged) {
         updateHScroller(true);
	     _document->status.maxColChanged = false;
      }
      else updateHScroller(resized);
   }
   refresh();
}

void TextView :: updateVScroller(bool resize)
{
   if (_document) {
      int position = getVScrollerPosition();

      if (resize) {
         Point size = _document->getSize();
         int   max = _document->getRowCount();
         _setScrollInfo(SB_VERT, max + size.y, size.y);
      }
      _setScrollPosition(SB_VERT, position);
   }
}

void TextView :: updateHScroller(bool resize)
{
   if (_document) {
      int position = getHScrollerPosition();

      if (resize) {
         Point size = _document->getSize();
         int max = _document->getMaxColumn();
         _setScrollInfo(SB_HORZ, max - 1, size.x);
      }
      _setScrollPosition(SB_HORZ, position);
   }
}

LRESULT TextView :: _WindProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
   switch (Message)
   {
      case WM_PAINT:
         onPaint();
         return 0;
      case WM_CHAR:
         if (onKeyPressed((wchar_t)wParam)) {
            return 0;
         }
         else break;
      case WM_KEYDOWN:
         if (onKeyDown((int)wParam, isKeyDown(VK_SHIFT), isKeyDown(VK_CONTROL))) {
            return 0;
         }
         else break;
      case WM_VSCROLL:
         onScroll(SB_VERT, LOWORD(wParam));
         return 0;
      case WM_HSCROLL:
         onScroll(SB_HORZ, LOWORD(wParam));
         return 0;
      case WM_MOUSEWHEEL:
         _onMouseWheel(HIWORD(wParam), (wParam & MK_CONTROL) != 0);
         return 0;
      case WM_MOUSEMOVE:
         _onMouseMove(Point(LOWORD(lParam), HIWORD(lParam)), (wParam & MK_LBUTTON) != 0);
         return 0;
      case WM_LBUTTONDOWN:
         _onButtonDown(Point(LOWORD(lParam), HIWORD(lParam)), (wParam & MK_SHIFT) != 0);
         return 0;
      case WM_LBUTTONUP:
         _onButtonUp();
         return 0;
      case WM_CONTEXTMENU:
         _onContextMenu((HWND)wParam, LOWORD(lParam), HIWORD(lParam));
         break;
      case WM_LBUTTONDBLCLK:
         _onDoubleClick();
         return 0;
   }
   return Window::_WindProc(hWnd, Message, wParam, lParam);
}

bool TextView :: onKeyPressed(wchar_t ch)
{
   if (!_document || _document->status.readOnly)
      return false;

   if (ch == 0x0D) {
      _document->insertNewLine();
   }
   else if (ch==0x08) {
      _document->eraseChar(true);
   }
   else if (ch==0x09) {
      indent();
   }
   else if (ch >= 0x20) {
      _document->insertChar(ch);
   }
   else return false;

   onEditorChange(); 
   return true;
}

bool TextView :: onKeyDown(int keyCode, bool kbShift, bool kbCtrl)
{
   if (!_document)
      return false;

   switch (keyCode) {
      case VK_LEFT:
         if (kbCtrl) {
            _document->moveLeftToken(kbShift);
         }
         else _document->moveLeft(kbShift);
         break;
      case VK_RIGHT:
         if (kbCtrl) {
            _document->moveRightToken(kbShift);
         }
         else _document->moveRight(kbShift);
         break;
      case VK_UP:
         if (kbCtrl) {
            _document->moveFrameUp();
         }
         else _document->moveUp(kbShift);
         break;
      case VK_DOWN:
         if (kbCtrl) {
            _document->moveFrameDown();
         }
         else _document->moveDown(kbShift);
         break;
      case VK_HOME:
         if (kbCtrl) {
            _document->moveFirst(kbShift);
         }
         else _document->moveHome(kbShift);
         break;
      case VK_END:
         if (kbCtrl) {
            _document->moveLast(kbShift);
         }
         else _document->moveEnd(kbShift);
         break;
      case VK_DELETE:
         if (_document->status.readOnly)
            return false;
            
         _document->eraseChar(false);
         break;
      case VK_PRIOR:
         _document->movePageUp(kbShift);
         break;
      case VK_NEXT:
         _document->movePageDown(kbShift);
         break;
      case VK_INSERT:
         if (!_document->status.readOnly) {
            _document->setOverwriteMode(!_document->status.overwriteMode);
            onSetFocus();
         }
         break;
      default:
         return false;
   }
   onEditorChange(); 
   return true;
}

void TextView :: onScroll(int bar, int type)
{
   SCROLLINFO info;

   _getScrollInfo(bar, &info);
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
/*
      case SB_TOP: topLineNew = 0; break;
	  case SB_BOTTOM: topLineNew = MaxScrollPos(); break;
*/
   }
   if (bar == SB_VERT) {
      _document->vscroll(offset);
   }
   else _document->hscroll(offset);

   onEditorChange(/*false*/);
}

void TextView :: _onButtonUp()
{
   _releaseMouse();
}

void TextView :: _onButtonDown(Point point, bool kbShift)
{
   setFocus();

   if (_document) {
      int col = 0, row = 0;
      bool margin = false;
      mouseToScreen(point, col, row, margin);

      _document->moveToFrame(col, row, kbShift);
      if (margin) {
         _notify(_receptor->getHandle(), TEXT_VIEW_MARGINCLICKED);
      }
      _captureMouse();

   //   refresh(true);
      onEditorChange(/*false*/);
   }
}

void TextView :: _onMouseMove(Point point, bool kbLButton)
{
   if (kbLButton && _isMouseCaptured() && _document != NULL) {
      int col = 0, row = 0;
      bool margin = false;
      mouseToScreen(point, col, row, margin);

      _document->moveToFrame(col, row, true);

      onEditorChange();
   }
}

void TextView :: _onMouseWheel(short wheelDelta, bool kbCtrl)
{
   if (_document) {
      int offset = (wheelDelta > 0) ? -1 : 1;

      if (kbCtrl) {
         offset *= _document->getSize().y;
      }
      _document->vscroll(offset);

      onEditorChange();
   }
}

void TextView :: _onDoubleClick()
{
   if (_document) {
      _document->selectWord();
      
      onEditorChange();
   }
}

void TextView :: onResize()
{
   Window::onResize();

   _zbuffer.release();

   resizeDocument();
   update(true);
}

bool TextView :: _onSetCursor()
{
   POINT pt;
   ::GetCursorPos(&pt);
   ::ScreenToClient(_handle, &pt);

   int col, row;
   bool margin = false;
   mouseToScreen(_GUI_::Point(pt.x, pt.y), col, row, margin);

   if (margin) {
      _setCursor(CURSOR_ARROW);
   }
   else _setCursor(CURSOR_TEXT);

   return true;
}

void TextView :: onSetFocus()
{
   if (_document) {
      //if (!_caretValid) {      
         _caretVisible = true;
         createCaret(_styles.getLineHeight(), _document->status.overwriteMode ? _styles[0].avgCharWidth : 1);
         showCaret();
         refresh();
      //}
   }
}

void TextView :: onLoseFocus()
{
   destroyCaret();
   _caretVisible = _caretValid = false;
   _caret_x = 0;
}

void TextView :: onPaint()
{
   if (_document) {
      if (_caretValid && _caretVisible)
         hideCaret();

      PAINTSTRUCT ps;
      Rectangle   clientRect = getRectangle();

      ::BeginPaint(_handle, &ps);

      Canvas canvas(ps.hdc);

      if (_zbuffer.isReleased()) {
         _cached = false;
         _zbuffer.clone(&canvas, clientRect.Width(), clientRect.Height());
      }

      paint(_zbuffer, clientRect);
      canvas.copy(clientRect, Point(0, 0), _zbuffer);

      ::EndPaint(_handle, &ps);

      if (_caretValid && _caretVisible)
         showCaret();
   }
}

void TextView :: onEditorChange()
{
   if (_document && _document->status.isViewChanged()) {
      _cached = false;

      _caret_x = 0;
   }

   update();

   if (_receptor)
      _notify(_receptor->getHandle(), TEXT_VIEW_CHANGED);
}

void TextView :: _onContextMenu(HWND, short x, short y)
{
   if (_receptor) {
      ContextMenuNMHDR notification;
      notification.nmhrd.code = CONTEXT_MENU_ON;
      notification.nmhrd.hwndFrom = _handle;
      notification.x = x;
      notification.y = y;

      ::SendMessage(_receptor->getHandle(), WM_NOTIFY, 0, (LPARAM)&notification);
   }
}

void TextView :: paint(Canvas& canvas, _GUI_::Rectangle clientRect)
{
   Point caret = _document->getCaret(false) - _document->getFrame();

   Style defaultStyle = _styles[STYLE_DEFAULT];
   Style marginStyle = _styles[STYLE_MARGIN];
   int lineHeight = _styles.getLineHeight();
   int marginWidth = _styles.getMarginWidth();
   if (_lineNumbersVisible) {
      marginWidth += getLineNumberMargin();
   }

   if (!_cached) {
      if (!defaultStyle.valid) {
         _styles.validate(&canvas);

         // reset style dependent variables
         lineHeight = _styles.getLineHeight();
         defaultStyle = _styles[STYLE_DEFAULT];
         marginStyle = _styles[STYLE_MARGIN];

         _needToResize = true;         
      }
      // if document size yet to be defined
      if (_needToResize)
         resizeDocument();

      // Draw background
      clientRect.bottomRight += Point(1, 1);
      canvas.fillRectangle(clientRect, defaultStyle);

      // Draw margin
      int marginWidth = _styles.getMarginWidth();
      if (_lineNumbersVisible) {
         marginWidth += getLineNumberMargin();
      }
      Rectangle margin(clientRect.topLeft.x, clientRect.topLeft.y, 
	      marginWidth, clientRect.Height());

      canvas.fillRectangle(margin, marginStyle);

      // Draw text
      canvas.setClipArea(clientRect);

      clientRect.bottomRight -= Point(1, 1);

      int x = clientRect.topLeft.x + marginWidth;
      int y = clientRect.topLeft.y - lineHeight + 1;
      int width = 0;
      _ELENA_::String<wchar_t, 6> lineNumber;

      Style style = defaultStyle;
      wchar_t buffer[0x100];
      int   length = 0;

      _ELENA_::LiteralWriter<wchar_t> writer(buffer, 0xFF);
      Document::Reader reader(_document);
      
      reader.readFirst(writer, 0xFF);
      do {
         style = _styles[reader.style];
         length = writer.Position();

         if (reader.newLine) {
            reader.newLine = false;

            x = clientRect.topLeft.x + marginWidth;
            y += lineHeight;

            if (_lineNumbersVisible) {
               lineNumber.copyInt(reader.row + 1);
               canvas.drawTextClipped(
                     Rectangle(x - marginWidth, y, marginWidth, lineHeight + 1),
                     x - marginStyle.avgCharWidth * (int)_ELENA_::getlength(lineNumber) - 4,
                     y,
                     lineNumber,
                     (int)_ELENA_::getlength(lineNumber),
                     marginStyle);
            }
            
            // !! HOTFIX: allow to see breakpoint ellipse on margin if STYLE_TRACELINe set for this line
            if (reader.marker) {
               canvas.drawEllipse(Rectangle(3, y + 2, 12, 12), style);

               reader.marker = false;
            }
            //   (info.bandLine && _currentDoc->checkMarker(info.bookmark.getRow(), STYLE_BREAKPOINT))) 
            //{
            //   canvas.drawEllipse(Rectangle(3, y + 2, 12, 12), style);
            //}
         }
         if (reader.bandStyle) {
            canvas.fillRectangle(Rectangle(x, y, clientRect.bottomRight.x - x, lineHeight + 1), style);

            reader.bandStyle = false;
         }

         width = canvas.TextWidth(&style, buffer, length);

         //if (defaultStyle._background != style._background) {
         if (length==0 && reader.style==STYLE_SELECTION) {
            canvas.drawTextClipped(Rectangle(x, y, style.avgCharWidth + 1, lineHeight + 1), x, y, 
			                          _T(" "), 1, style);
         }
         else canvas.drawTextClipped(Rectangle(x, y, width + 1, lineHeight + 1), x, y, 
			      buffer, length, style);
         //}
         //else canvas.drawTextClippedTransporent(Rectangle(x, y, width, lineHeight), x, y, info.line, info.length, style);

         x += width;
         writer.reset();
      }
      while (reader.readNext(writer, 0xFF));
   
      _cached = true;
   }

   if (caret.x >= 0 && caret.y >= 0) {
      if ((_caret_x == 0) || _document->status.caretChanged) {
         _caret_x = 0;

         wchar_t buffer[0x100];
         _ELENA_::LiteralWriter<wchar_t> writer(buffer, 0xFF);

         Document::Reader reader(_document);
         reader.initCurrentLine();
         reader.readCurrentLine(writer, 0xFF);
         do {
            _caret_x += canvas.TextWidth(&_styles[reader.style], buffer, writer.Position());

            writer.reset();
         } while (reader.readCurrentLine(writer, 0xFF));

         _document->status.caretChanged = false;
      }
      
      _locateCaret(clientRect.topLeft.x + marginWidth + _caret_x,
          lineHeight * caret.y + 1);

      _caretVisible = true;
   }
   else _caretVisible = false;
}

int TextView :: getLineNumberMargin()
{
   if (_lineNumbersVisible) {
      Style marginStyle = _styles[STYLE_MARGIN];

      return marginStyle.avgCharWidth * 5;
   }
   else return 0;
}

void TextView :: createCaret(int height, int width)
{
   _caretValid = true;
   ::CreateCaret(_handle, NULL, width, height);
}

void TextView :: destroyCaret()
{
   ::DestroyCaret();
   _caretValid = false;
}

void TextView :: _locateCaret(int x, int y)
{
   ::SetCaretPos(x, y);
}

void TextView :: showCaret()
{
    ::ShowCaret(_handle);
}

void TextView :: hideCaret()
{
   ::HideCaret(_handle);
}

void TextView :: resizeDocument()
{
   if (_styles.Count() > 0 && _document != NULL) {
      Point     size;
      Rectangle client = getRectangle();

      int marginWidth = getLineNumberMargin();
      size.x = (client.Width() - marginWidth) / _styles[0].avgCharWidth;
      size.y = client.Height() / _styles.getLineHeight();

      _document->resize(size);

      _needToResize = false;
   }
   _cached = false;
}

void TextView :: setDocument(Document* document)
{
   _document = document;

   _document->setHighlightMode(_highlight);

   _needToResize = true;
   _cached = false;
   refresh();
}

void TextView :: mouseToScreen(Point point, int& col, int& row, bool& margin)
{
   Rectangle rect = getRectangle();
   Style defaultStyle = _styles[STYLE_DEFAULT];
   int marginWidth = _styles.getMarginWidth() + getLineNumberMargin();
   int offset = defaultStyle.avgCharWidth / 2;

   col = (point.x - rect.topLeft.x - marginWidth + offset) / defaultStyle.avgCharWidth;
   row = (point.y - rect.topLeft.y) / (_styles.getLineHeight());
   margin = (point.x - rect.topLeft.x < marginWidth);
}

int TextView :: getHScrollerPosition()
{
   return _document->getFrame().x;
}

int TextView :: getVScrollerPosition()
{
   return _document->getFrame().y;
}

bool TextView :: _getScrollInfo(int bar, SCROLLINFO* info)
{
   memset(info, 0, sizeof(*info));
   info->cbSize = sizeof(*info);
   info->fMask = SIF_ALL;

   return ::GetScrollInfo(_handle, bar, info) ? true : false;
}

void TextView :: _setScrollInfo(int bar, int max, int page)
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

void TextView :: _setScrollPosition(int bar, int position)
{
   ::SetScrollPos(_handle, bar, position, TRUE);
}

void TextView :: indent()
{
   if (_tabUsing) {
      _document->tabbing('\t', 1, true);
   }
   else {
      if (!_document->hasSelection()) {
         int shift = _ELENA_::calcTabShift(_document->getCaret().x, _tabSize);
         _document->insertChar(' ', shift);
      } 
      else _document->tabbing(' ', _tabSize, true);
   }
   refreshView();
}

void TextView :: outdent()
{
   if (_document->hasSelection()) {
      if (_tabUsing) {
         _document->tabbing('\t', 1, false);
      }
      else _document->tabbing(' ', _tabSize, false);
   }
   refreshView();
}

void TextView :: _captureMouse()
{
   _mouseCaptured = true;
   ::SetCapture(_handle);
}

void TextView :: _releaseMouse()
{
   _mouseCaptured = false;
   ::ReleaseCapture();
}
