//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      DocumentView class body
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "guicommon.h"
// --------------------------------------------------------------------------
#include "document.h"
#include "guieditor.h"
#include <tchar.h>

using namespace elena_lang;

constexpr auto WHITESPACE = _T(" \r\t");
constexpr auto OPERATORS = _T("(){}[]:=<>.,^@+-*/!~?;\"");

constexpr auto UNDO_BUFFER_SIZE = 0x40000;


// --- LexicalFormatter ---

LexicalFormatter :: LexicalFormatter(Text* text, TextFormatterBase* formatter, MarkerList* markers)
{
   _text = text;
   _markers = markers;
   _formatter = formatter;

   _text->attachWatcher(this);

   onUpdate(-1);
}

LexicalFormatter :: ~LexicalFormatter()
{
   _text->detachWatcher(this);
}

void LexicalFormatter :: format()
{
   _indexes.clear();
   _lexical.clear();

   MemoryWriter  indexWriter(&_indexes);
   MemoryWriter  writer(&_lexical);
   pos_t         indexedPos = INDEX_STEP;

   text_t        s = nullptr;
   pos_t         length = 0;
   FormatterInfo info;

   TextBookmarkReader reader(_text);

   indexWriter.writePos(0);
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

bool LexicalFormatter :: checkMarker(ReaderInfo& info)
{
   auto it = _markers->getIt(info.row + 1);
   if (!it.eof()) {
      auto marker = *it;

      info.bandStyle = true;
      info.style = marker.style;
      info.step = 0;

      return true;
   }
   return false;
}

pos_t LexicalFormatter :: proceed(pos_t position, ReaderInfo& info)
{
   pos_t count = 0;
   if (info.newLine && checkMarker(info)) {
      return INVALID_POS;
   }

   if (info.step == 0) {
      info.step = retrievePosition(position);
   }

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

DocumentView :: DocumentView(Text* text, TextFormatterBase* formatter) :
   _undoBuffer(UNDO_BUFFER_SIZE),
   _formatter(text, formatter, &_markers),
   _notifiers(nullptr),
   _markers({})
{
   _text = text;

   _maxColumn = 0;
   _selection = 0;

   _text->attachWatcher(this);
}

DocumentView :: ~DocumentView()
{
   _text->detachWatcher(this);
}

void DocumentView :: onInsert(size_t position, size_t length, text_t line)
{
   _undoBuffer.onInsert(position, length, line);

   _frame.invalidate();

   if (_caret.longPosition() > position) {
      _caret.invalidate();
   }

   status.modifiedMode = true;
   //status.frameChanged = true;
}

void DocumentView :: onUpdate(size_t position)
{
   _undoBuffer.onUpdate(position);

   _frame.invalidate();

   if (_caret.longPosition() > position) {
      _caret.invalidate();
   }

   status.modifiedMode = true;
   //status.frameChanged = true;
}

void DocumentView :: onErase(size_t position, size_t length, text_t line)
{
   _undoBuffer.onErase(position, length, line);

   _frame.invalidate();

   if (_caret.longPosition() > position) {
      _caret.invalidate();
   }

   status.modifiedMode = true;
   //status.frameChanged = true;
}

disp_t DocumentView :: getSelectionLength()
{
   return abs(_selection);
}

pos_t DocumentView :: format(LexicalReader& reader)
{
   pos_t length = _size.x;
   pos_t position = reader.bm.position();

   if (_selection != 0) {
      pos_t curPos = _caret.position();
      pos_t selPos = curPos + _selection;
      if (_selection > 0) {
         if (reader.bm.row() == _caret.row() && position < curPos) {
            length = _min(curPos - position, length);
         }
         else if (position >= curPos && position < selPos) {
            reader.style = STYLE_SELECTION;

            return _min(selPos - position, length);
         }
      }
      else {
         if (position >= selPos && position < curPos) {
            reader.style = STYLE_SELECTION;

            return _min(curPos - position, length);
         }
         else if (position < selPos && position + length > selPos) {
            length = _min(selPos - position, length);
         }
      }
   }

   pos_t proceeded = _formatter.proceed(position, reader);
   return _min(length, proceeded);
}

void DocumentView :: setSize(Point size)
{
   _size = size;
   if (_size.x == 0 && _size.y == 0) {
      _caret.moveTo(0, 0);
      _frame.moveTo(0, 0);

      return;
   }

   DocumentChangeStatus dummy = {};
   setCaret(_caret.getCaret(), false, dummy);
}

void DocumentView :: setCaret(int column, int row, bool selecting, DocumentChangeStatus& changeStatus)
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
   if (_maxColumn < _caret.length_int() + _size.x) {
      _maxColumn = _caret.length_int() + _size.x;

      changeStatus.maxColChanged = true;
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
      frame.y = caret.y - _size.y + 3;
   }

   if (_frame.getCaret() != frame) {
      _text->validateBookmark(_frame);
      _frame.moveTo(frame.x, frame.y);

      changeStatus.frameChanged = true;
   }
   if (selecting) {
      _selection += position - _caret.position();

      changeStatus.hasSelection = _selection != 0;
   }
   else {
      _selection = 0;
   }

   changeStatus.caretChanged = true;
}

void DocumentView :: hscroll(DocumentChangeStatus& changeStatus, int displacement)
{
   Point frame = _frame.getCaret();

   frame.x += displacement;
   if (frame.x < 0)
      frame.x = 0;

   if (_frame.getCaret() != frame) {
      _frame.moveTo(frame.x, frame.y);

      changeStatus.frameChanged = true;
   }
}

void DocumentView :: vscroll(DocumentChangeStatus& changeStatus, int displacement)
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

      changeStatus.frameChanged = true;
   }
}

void DocumentView :: moveLeft(DocumentChangeStatus& changeStatus, bool selecting)
{
   if (selecting) {
      pos_t position = _caret.position();
      _caret.moveOn(-1);

      _selection += position - _caret.position();

      changeStatus.hasSelection = _selection != 0;
   }
   else _caret.moveOn(-1);

   setCaret(_caret.getCaret(), selecting, changeStatus);
}

void DocumentView :: moveRight(DocumentChangeStatus& changeStatus, bool selecting)
{
   if (selecting) {
      pos_t position = _caret.position();
      _caret.moveOn(1);

      _selection += position - _caret.position();

      changeStatus.hasSelection = _selection != 0;
   }
   else _caret.moveOn(1);

   setCaret(_caret.getCaret(), selecting, changeStatus);
}

void DocumentView :: moveUp(DocumentChangeStatus& changeStatus, bool selecting)
{
   if (_caret.row() > 0) {
      setCaret(_caret.column(), _caret.row() - 1, selecting, changeStatus);
   }
}

void DocumentView :: moveDown(DocumentChangeStatus& changeStatus, bool selecting)
{
   if (_caret.row() < _text->getRowCount()) {
      setCaret(_caret.column(), _caret.row() + 1, selecting, changeStatus);
   }
}

void DocumentView :: moveLeftToken(DocumentChangeStatus& changeStatus, bool selecting)
{
   text_str ws(WHITESPACE);
   text_str operators(OPERATORS);

   bool newToken = false;
   bool operatorOne = false;
   pos_t position = _caret.position();

   _caret.moveOn(-1);

   while (_caret.column() > 0 || _caret.row() > 0) {
      if (_caret.column() == _caret.length_int())
         break;

      pos_t length = 0;
      text_t line = _text->getLine(_caret, length);
      if (length == 0)
         break;

      if (ws.find(line[0]) != NOTFOUND_POS) {
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

      _caret.moveOn(-1);
   }
   if (selecting) {
      _selection += position - _caret.position();

      changeStatus.hasSelection = _selection != 0;
   }
   setCaret(_caret.getCaret(), selecting, changeStatus);
}

void DocumentView :: moveRightToken(DocumentChangeStatus& changeStatus, bool selecting, bool trimWhitespace)
{
   text_str ws(WHITESPACE);
   text_str operators(OPERATORS);

   pos_t position = _caret.position();
   bool newToken = false;
   bool operatorOne = false;
   bool first = true;
   while (first || (pos_t)_caret.column() < _caret.position()) {
      pos_t length = 0;
      text_t line = _text->getLine(_caret, length);
      if (length == 0)
         break;

      if (ws.find(line[0]) != NOTFOUND_POS) {
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

      changeStatus.hasSelection = _selection != 0;
   }
   setCaret(_caret.getCaret(), selecting, changeStatus);
}

void DocumentView :: moveFrameUp(DocumentChangeStatus& changeStatus)
{
   vscroll(changeStatus, -1);
   if (changeStatus.frameChanged) {
      if (_frame.row() + _size.y - 2 <= _caret.row()) {
         setCaret(_caret.column(), _frame.row() + _size.y - 3, false, changeStatus);
      }
   }
}

void DocumentView :: moveFrameDown(DocumentChangeStatus& changeStatus)
{
   vscroll(changeStatus, 1);
   if (changeStatus.frameChanged) {
      if (_caret.row() < _frame.row()) {
         setCaret(_caret.column(), _frame.row(), false, changeStatus);
      }
   }
}

void DocumentView :: moveFirst(DocumentChangeStatus& changeStatus, bool selecting)
{
   setCaret(0, 0, selecting, changeStatus);
}

void DocumentView :: moveEnd(DocumentChangeStatus& changeStatus, bool selecting)
{
   int lastRow = _text->getRowCount() - 1;
   setCaret((int)_text->getRowLength(lastRow), lastRow, selecting, changeStatus);
}

void DocumentView :: moveHome(DocumentChangeStatus& changeStatus, bool selecting)
{
   setCaret(0, _caret.row(), selecting, changeStatus);
}

void DocumentView :: moveLast(DocumentChangeStatus& changeStatus, bool selecting)
{
   setCaret(_caret.length_int(), _caret.row(), selecting, changeStatus);
}

void DocumentView :: moveToFrame(DocumentChangeStatus& changeStatus, int column, int row, bool selecting)
{
   setCaret(_frame.column() + column, _frame.row() + row, selecting, changeStatus);
}

void DocumentView :: movePageDown(DocumentChangeStatus& changeStatus, bool selecting)
{
   if (_caret.row() + _size.y > _text->getRowCount() - 1) {
      setCaret(_caret.column(), _text->getRowCount() - 1, selecting, changeStatus);
   }
   else {
      vscroll(changeStatus, _size.y);
      if (changeStatus.frameChanged)
         setCaret(_caret.column(), _caret.row() + _size.y, selecting, changeStatus);
   }
}

void DocumentView :: movePageUp(DocumentChangeStatus& changeStatus, bool selecting)
{
   if (_caret.row() == 0)
      return;

   if (_frame.row() == 0) {
      setCaret(_caret.column(), 0, selecting, changeStatus);
   }
   else {
      vscroll(changeStatus, -_size.y);
      if (changeStatus.frameChanged)
         setCaret(_caret.column(), _caret.row() - _size.y, selecting, changeStatus);
   }
}

void DocumentView :: notifyOnChange(DocumentChangeStatus& changeStatus)
{
   if (status.oldModified != status.modifiedMode) {
      changeStatus.modifiedChanged = true;
      status.oldModified = status.modifiedMode;
   }
   bool isSelected = changeStatus.hasSelection;
   if (isSelected != status.oldSelected) {
      changeStatus.selelectionChanged = true;
      status.oldSelected = isSelected;
   }

   for(auto it = _notifiers.start(); !it.eof(); ++it) {
      (*it)->onDocumentUpdate(changeStatus);
   }
}

void DocumentView :: blockInserting(DocumentChangeStatus& changeStatus, text_t subs, size_t length)
{
   _text->validateBookmark(_caret);

   if (_selection < 0) {
      _caret.moveOn(_selection);
      _selection = abs(_selection);
   }
   TextBookmark end = _caret;

   end.moveOn(_selection);

   // if only part of the line was selected just insert tab
   int lastRow = end.row();
   if (lastRow == _caret.row()) {
      _text->insertLine(_caret, subs, length);

      changeStatus.textChanged = true;
   }
   else {
      setCaret(0, _caret.row(), true, changeStatus);

      if (end.column() == 0)
         lastRow--;

      TextBookmark start = _caret;
      while (start.row() <= lastRow) {
         _text->insertLine(start, subs, length);

         changeStatus.textChanged = true;

         if (!start.moveTo(0, start.row() + 1))
            break;
      }
   }
}

void DocumentView :: tabbing(DocumentChangeStatus& changeStatus, text_c space, size_t count, bool indent)
{
   _text->validateBookmark(_caret);

   if (_selection < 0) {
      _caret.moveOn(_selection);
      _selection = abs(_selection);
   }
   TextBookmark end = _caret;

   end.moveOn(_selection);

   // if only part of the line was selected just insert tab
   int lastRow = end.row();
   if (lastRow == _caret.row()) {
      if (indent)
         insertChar(changeStatus, space, count);
   }
   else {
      setCaret(0, _caret.row(), true, changeStatus);

      if (end.column() == 0)
         lastRow--;

      TextBookmark start = _caret;
      while (start.row() <= lastRow) {
         if (indent) {
            for (size_t i = 0; i < count; i++) {
               _text->insertChar(start, space);
               changeStatus.textChanged = true;
               _selection++;
            }
         }
         else {
            pos_t length;
            for (size_t i = 0; i < count; i++) {
               text_t s = _text->getLine(start, length);
               if (length != 0 && (s[0] == ' ' || s[0] == '\t')) {
                  bool tab = (s[0] == '\t');

                  _text->eraseChar(start);
                  _selection--;
                  changeStatus.textChanged = true;

                  if (tab)
                     break;
               }
               else break;
            }
         }
         if (!start.moveTo(0, start.row() + 1))
            break;
      }
   }
}

void DocumentView :: insertChar(DocumentChangeStatus& changeStatus, text_c ch, size_t count)
{
   if (hasSelection()) {
      int rowCount = _text->getRowCount();

      eraseSelection(changeStatus);

      status.rowDifference += (_text->getRowCount() - rowCount);
   }
   else if (status.overwriteMode && (int)_caret.length() > _caret.column(false)) {
      _text->eraseChar(_caret);

      changeStatus.textChanged = true;
   }

   while (count > 0) {
      if (_text->insertChar(_caret, ch)) {
         changeStatus.textChanged = true;

         _text->validateBookmark(_caret);
         _caret.moveOn(1);
         setCaret(_caret.getCaret(), false, changeStatus);
      }
      else break;

      count--;
   }
}

void DocumentView :: insertNewLine(DocumentChangeStatus& changeStatus)
{
   int rowCount = _text->getRowCount();

   eraseSelection(changeStatus);

   if (_text->insertNewLine(_caret)) {
      setCaret(0, _caret.row() + 1, false, changeStatus);

      changeStatus.textChanged = true;
   }

   status.rowDifference += (_text->getRowCount() - rowCount);
}

void DocumentView :: insertLine(DocumentChangeStatus& changeStatus, text_t text, disp_t length)
{
   int rowCount = _text->getRowCount();

   eraseSelection(changeStatus);

   _text->insertLine(_caret, text, length);
   _caret.moveOn(length);
   changeStatus.textChanged = true;

   setCaret(_caret.getCaret(), false, changeStatus);

   status.rowDifference += (_text->getRowCount() - rowCount);
}

void DocumentView :: eraseChar(DocumentChangeStatus& changeStatus, bool moveback)
{
   int rowCount = _text->getRowCount();

   if (_selection != 0) {
      eraseSelection(changeStatus);

      setCaret(_caret.getCaret(), false, changeStatus);
   }
   else {
      if (moveback) {
         if (_caret.column(false) == 0 && _caret.row() == 0)
            return;

         moveLeft(changeStatus, false);
      }
      _text->eraseChar(_caret);
      changeStatus.textChanged = true;
   }

   status.rowDifference += (_text->getRowCount() - rowCount);
}

bool DocumentView :: eraseSelection(DocumentChangeStatus& changeStatus)
{
   if (_selection == 0)
      return false;

   _text->validateBookmark(_caret);

   if (_selection < 0) {
      _caret.moveOn(_selection);
      _selection = -_selection;
   }
   _text->eraseLine(_caret, _selection);
   _selection = 0;
   changeStatus.textChanged = true;

   return true;
}

void DocumentView :: copySelection(text_c* text)
{
   if (_selection == 0) {
      text[0] = 0;
   }
   else {
      _text->copyTo(_caret, text, _selection);
   }
}

void DocumentView :: undo(DocumentChangeStatus& changeStatus)
{
   changeStatus.textChanged |= _undoBuffer.undo(_text, _caret);

   setCaret(getCaret(false), false, changeStatus);
}

void DocumentView :: redo(DocumentChangeStatus& changeStatus)
{
   changeStatus.textChanged |= _undoBuffer.redo(_text, _caret);

   setCaret(getCaret(false), false, changeStatus);
}

void DocumentView :: save(path_t path)
{
   _text->save(path);

   status.modifiedMode = false;

   DocumentChangeStatus changeStatus = {};
   notifyOnChange(changeStatus);
}

bool DocumentView :: canRedo()
{
   return !_undoBuffer.eof();
}

bool DocumentView :: canUndo()
{
   return !_undoBuffer.bof();
}
