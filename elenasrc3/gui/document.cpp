//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      DocumentView class body
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "guicommon.h"
// --------------------------------------------------------------------------
#include "document.h"

using namespace elena_lang;

constexpr auto WHITESPACE = _T(" \r\t");
constexpr auto OPERATORS = _T("(){}[]:=<>.,^@+-*/!~?;\"");

// --- LexicalFormatter ---

LexicalFormatter :: LexicalFormatter(Text* text, TextFormatterBase* formatter)
{
   _text = text;
   _formatter = formatter;

   _text->attachWatcher(this);

   onUpdate(-1);
}

LexicalFormatter :: ~LexicalFormatter()
{
   _text->dettachWatcher(this);
}

void LexicalFormatter :: format()
{
   _indexes.clear();

   MemoryWriter  indexWriter(&_indexes);
   MemoryWriter  writer(&_lexical);
   pos_t         indexedPos = INDEX_STEP;

   text_t        s = nullptr;
   pos_t         length = 0;
   FormatterInfo info;

   TextBookmarkReader reader(_text);

   indexWriter.writeSize(0);
   _formatter->start(info);
   pos_t        style = info.style;
   while (true) {
      if (reader.position() == indexedPos) {
         indexWriter.writePos(writer.position());
         indexedPos += INDEX_STEP;
      }

      s = reader.readLine(length);
      for (pos_t i = 0; i < length; i++) {
         if(_formatter->next(s[i], info, style)) {
            writer.writePos(style);
            writer.writePos(reader.position() + i);
         }
      }

      if (!reader.goTo((disp_t)length))
         break;
   }
   writer.writePos(style);
   writer.writePos(reader.position() + length);
}

pos_t LexicalFormatter :: proceed(pos_t position, ReaderInfo& info)
{
   if (info.step == 0) {
      info.step = retrievePosition(position);
   }

   pos_t count = 0;
   MemoryReader reader(&_lexical, info.step);
   pos_t curStyle = 0;
   pos_t current = 0;
   while (reader.readPos(curStyle)) {
      current = reader.getPos();
      if (current > position) {
         info.style = curStyle;
         count = current - position;
         break;
      }
      info.step = reader.position();
   }

   return count;
}

void LexicalFormatter :: onUpdate(size_t)
{
   format();
}

void LexicalFormatter :: onInsert(size_t, size_t, text_t)
{
}

void LexicalFormatter  :: onErase(size_t, size_t, text_t)
{
}

// --- DocumentView::LexicalReader ---

void DocumentView::LexicalReader :: seekCurrentLine()
{
   style = step = 0;
   newLine = true;

   bm = docView->getCaretBookmark();

   Point caret = docView->getCaret(false);
   Point frame = docView->getFrame();

   region.topLeft = Point(frame.x, caret.y);
   region.bottomRight = Point(caret.x, caret.y);

   bm.moveTo(frame.x, caret.y);

   docView->_text->validateBookmark(bm);
   row = bm.row();
}

bool DocumentView::LexicalReader :: readCurrentLine(TextWriter<text_c>& writer, pos_t length)
{
   style = 0;

   if (bm.isEOF())
      return false;

   int column = region.bottomRight.x;
   if ((bm.column() < column) && !bm.isEOL()) {
      pos_t styleLength = docView->format(*this);
      if (styleLength < length)
         length = styleLength;

      docView->_text->copyLineToX(bm, writer, length, column);

      return true;
   }
   else return false;
}

void DocumentView::LexicalReader :: readFirst(TextWriter<text_c>& writer, pos_t length)
{
   style = step = 0;
   newLine = true;

   region.topLeft = docView->_frame.getCaret();
   region.bottomRight = region.topLeft + docView->_size;
   bm = docView->_frame;

   docView->_text->validateBookmark(bm);
   row = bm.row();

   // read first line
   if (docView->_text->getRowCount() != 0) {
      pos_t styleLength = docView->format(*this);
      if (styleLength < length)
         length = styleLength;

      docView->_text->copyLineTo(bm, writer, length, true);
   }
}

bool DocumentView::LexicalReader :: readNext(TextWriter<text_c>& writer, pos_t length)
{
   style = 0;

   if (bm.isEOF())
      return false;

   if (bm.column() >= region.bottomRight.x || bm.isEOL()) {
      if (bm.row() >= region.bottomRight.y)
         return false;

      bm.moveTo(region.topLeft.x, bm.row() + 1);
      newLine = true;
      row = bm.row();
   }

   pos_t styleLength = docView->format(*this);
   if (styleLength < length)
      length = styleLength;

   docView->_text->copyLineTo(bm, writer, length, true);

   return true;
}

// --- DocumentView ---

DocumentView :: DocumentView(Text* text, TextFormatterBase* formatter)
   : _formatter(text, formatter), _notifiers(nullptr)
{
   _text = text;

   _maxColumn = 0;
   _selection = 0;
}

pos_t DocumentView :: format(LexicalReader& reader)
{
   pos_t length = _size.x;
   pos_t position = reader.bm.position();

   pos_t proceeded = _formatter.proceed(position, reader);
   return min(length, proceeded);
}

void DocumentView :: resize(Point size)
{
   _size = size;
   if (_size.x == 0 && _size.y == 0) {
      _caret.moveTo(0, 0);
      _frame.moveTo(0, 0);

      return;
   }

   setCaret(_caret.getCaret(), false);
}

void DocumentView :: setCaret(int column, int row, bool selecting)
{
   if (column < 0) column = 0;
   if (row < 0) {
      row = 0;
   }
   else if (row >= _text->getRowCount())
      row = _text->getRowCount() - 1;

   _text->validateBookmark(_caret);

   pos_t position = _caret.position();

   _caret.moveTo(column, row);
   if (_maxColumn < _caret.length_pos() + _size.x) {
      _maxColumn = _caret.length_pos() + _size.x;

      status.maxColChanged = true;
   }

   Point frame = _frame.getCaret();
   Point caret = _caret.getCaret(false);
   if (caret.x < frame.x) {
      frame.x = caret.x;
   }
   else if (frame.x + _size.x - 2 <= caret.x) {
      frame.x = caret.x - _size.x + 3;
   }

   if (caret.y < frame.y) {
      frame.y = caret.y;
   }
   else if (frame.y + _size.y - 1 <= caret.y) {
      frame.y = caret.y - _size.y + 2;
   }

   if (_frame.getCaret() != frame) {
      _text->validateBookmark(_frame);
      _frame.moveTo(frame.x, frame.y);
      status.frameChanged = true;
   }
   if (selecting) {
      _selection += position - _caret.position();
      status.selelectionChanged = true;
   }
   else {
      if (_selection != 0)
         status.selelectionChanged = true;

      _selection = 0;
   }

   status.caretChanged = true;

   notifyOnChange();
}

void DocumentView :: vscroll(int displacement)
{
   Point frame = _frame.getCaret();

   frame.y += displacement;
   if (frame.y < 0)
      frame.y = 0;
   else if (frame.y > _text->getRowCount())
      frame.y = _text->getRowCount();

   if (_frame.getCaret() != frame) {
      _text->validateBookmark(_frame);
      _frame.moveTo(frame.x, frame.y);
      status.frameChanged = true;
   }
}

void DocumentView :: moveLeft(bool selecting)
{
   if (selecting) {
      pos_t position = _caret.position();
      _caret.moveOn(-1);

      _selection += position - _caret.position();
   }
   else _caret.moveOn(-1);

   setCaret(_caret.getCaret(), selecting);
}

void DocumentView :: moveRight(bool selecting)
{
   if (selecting) {
      pos_t position = _caret.position();
      _caret.moveOn(1);

      _selection += position - _caret.position();
   }
   else _caret.moveOn(1);

   setCaret(_caret.getCaret(), selecting);
}

void DocumentView :: moveUp(bool selecting)
{
   if (_caret.row() > 0) {
      setCaret(_caret.column(), _caret.row() - 1, selecting);
   }
}

void DocumentView :: moveDown(bool selecting)
{
   if (_caret.row() < _text->getRowCount()) {
      setCaret(_caret.column(), _caret.row() + 1, selecting);
   }
}

void DocumentView :: moveLeftToken(bool selecting)
{
   text_str ws(WHITESPACE);
   text_str operators(OPERATORS);

   bool newToken = false;
   bool operatorOne = false;
   pos_t position = _caret.position();

   _caret.moveOn(-1);

   while (_caret.column() > 0 || _caret.row() > 0) {
      if (_caret.column() == _caret.length_pos())
         break;

      pos_t length = 0;
      text_t line = _text->getLine(_caret, length);
      if (length == 0)
         break;

      if (ws.find(line[0] != NOTFOUND_POS)) {
         if (operatorOne || newToken) {
            _caret.moveOn(1);
            break;
         }
      }
      else if (operators.find(line[0]) != NOTFOUND_POS) {
         if (newToken) {
            _caret.moveOn(1);
            break;
         }
         else operatorOne = true;
      }
      else {
         if (operatorOne) {
            _caret.moveOn(1);
            break;
         }
         else newToken = true;
      }

      _caret.moveOn(1);
   }
   if (selecting) {
      _selection += position - _caret.position();
   }
   setCaret(_caret.getCaret(), selecting);
}

void DocumentView :: moveRightToken(bool selecting, bool trimWhitespace)
{
   text_str ws(WHITESPACE);
   text_str operators(OPERATORS);

   pos_t position = _caret.position();
   bool newToken = false;
   bool operatorOne = false;
   bool first = false;
   while (first || _caret.column() < _caret.position()) {
      pos_t length = 0;
      text_t line = _text->getLine(_caret, length);
      if (length == 0)
         break;

      if (ws.find(line[0] != NOTFOUND_POS)) {
         newToken = true;
         operatorOne = false;

         if (trimWhitespace)
            break;
      }
      else if (operators.find(line[0]) != NOTFOUND_POS) {
         if (!operatorOne && !first) {
            break;
         }
         operatorOne = true;
      }
      else if (newToken || operatorOne)
         break;

      _caret.moveOn(1);
      first = false;
   }
   if (selecting) {
      _selection += position - _caret.position();
   }
   setCaret(_caret.getCaret(), selecting);
}

void DocumentView :: moveFrameUp()
{
   vscroll(-1);
   if (status.frameChanged) {
      if (_frame.row() + _size.y - 2 <= _caret.row()) {
         setCaret(_caret.column(), _frame.row() + _size.y - 3, false);
      }
   }
}

void DocumentView :: moveFrameDown()
{
   vscroll(1);
   if (status.frameChanged) {
      if (_caret.row() < _frame.row()) {
         setCaret(_caret.column(), _frame.row(), false);
      }
   }
}

void DocumentView :: moveToFrame(int column, int row, bool selecting)
{
   setCaret(_frame.column() + column, _frame.row() + row, selecting);
}

void DocumentView :: notifyOnChange()
{
   for(auto it = _notifiers.start(); !it.eof(); ++it) {
      (*it)->onDocumentUpdate();
   }
}
