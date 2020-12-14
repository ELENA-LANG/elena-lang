//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Document class implementation
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#include "document.h"

using namespace _GUI_;

#define WHITESPACE        _T(" \r\t")
#define OPERATORS         _T("(){}[]:=<>.,^@+-*/!~?;\"")
#define TERMINATORS       _T(" (){}[]:=<>\r.,^@+-*/!~?\t;\"")

// --- LexicalStyler ---

LexicalStyler::LexicalStyler(Text* text, _ELENA_::pos_t defaultStyle, text_c lookaheadState, text_c startState,
   text_c(*makeStep)(text_c ch, text_c state), _ELENA_::pos_t(*defineStyle)(text_c state, _ELENA_::pos_t style))
{
   _text = text;
   _defaultStyle = defaultStyle;
   _lookaheadState = lookaheadState;
   _startState = startState;

   _makeStep = makeStep;
   _defineStyle = defineStyle;

   _text->attachWatcher(this);

   _enabled = false;

   onUpdate(-1);
}

void LexicalStyler :: setEnabled(bool enabled)
{
   bool changed = (_enabled != enabled);

   _enabled = enabled;
   if (changed)
      parse();
}

void LexicalStyler :: parse()
{
   if (!_enabled)
      return;

   _index.clear();
   _lexic.clear();

   _ELENA_::MemoryWriter indexWriter(&_index, 0);
   _ELENA_::MemoryWriter writer(&_lexic, 0);
   _ELENA_::pos_t        indexedPos = INDEX_STEP;

   text_c         state = _startState;
   text_t         s;
   size_t         length = 0;
   _ELENA_::pos_t style = _defaultStyle;
   bool           lookAhead = false;

   TextScanner scanner(_text);
   indexWriter.writeSize(0);
   while (true) {
	  if (scanner.getPosition() == indexedPos) {
         indexWriter.writeDWord(writer.Position());
         indexedPos += INDEX_STEP;
      }
      s = scanner.getLine(length);
      for (size_t i = 0 ; i < length ; i++) {
         state = _makeStep(s[i], state);
         if (lookAhead) {
            style = _defineStyle(state, style);
         }
         else if (style != _defineStyle(state, style) || state == _lookaheadState) {
            writer.writeDWord(style);
            writer.writeDWord(scanner.getPosition() + i);
            style = _defineStyle(state, style);
         }
         lookAhead = (state == _lookaheadState);
      }
      if (!scanner.goTo((disp_t)length))
         break;
   }
   writer.writeDWord(style);
   writer.writeDWord(scanner.getPosition() + length);
}

_ELENA_::pos_t LexicalStyler :: proceed(_ELENA_::pos_t position, LexicalInfo& info)
{
   if (!_enabled)
      return 0xFFFFFFFF;

   size_t count = 0;
   if (info.step==0) {
	  info.step = retrievePosition(position); // info.bookmark.getPosition()
   }
   _ELENA_::MemoryReader reader(&_lexic, info.step);
   _ELENA_::pos_t        curStyle = 0;
   _ELENA_::pos_t        current = 0;
   while (reader.readDWord(curStyle)) {
      reader.readDWord(current);
      if (current > position) {
         info.style = curStyle;
         count = current - position;
         break;
      }
      info.step = reader.Position();
   }
   return count;
}

// --- Document::Reader ---

bool Document::Reader :: readFirst(_ELENA_::TextWriter& writer, size_t length)
{
   // init bookmark
   step = 0;
   style = 0;
   newLine = true;
   bandStyle = false;

   _region.topLeft = _doc->_frame.getCaret();
   _region.bottomRight = _region.topLeft + _doc->_size;
   bm = _doc->_frame;

   _doc->_text->validateBookmark(bm);
   row = bm.getRow();

   // read first line
   if (_doc->_text->getRowCount()!=0) {
      //if ((size_t)info.frame.bottomRight.y >= _text->getRowCount()) {
      //   info.frame.bottomRight.y = _text->getRowCount() - 1;
      //}

      size_t styleLen = _doc->defineStyle(*this);
      if (styleLen < length)
         length = styleLen;

      _doc->_text->copyLineTo(bm, writer, length, true);
   }
   return true;
}

void Document::Reader :: initCurrentLine()
{
   // init bookmark
   step = 0;
   style = 0;
   newLine = true;
   bandStyle = false;

   bm = _doc->getCurrentTextBookmark();

   Point caret = _doc->getCaret(false);
   Point frame = _doc->getFrame();

   _region.topLeft = Point(frame.x, caret.y);
   _region.bottomRight = Point(caret.x, caret.y);

   bm.moveTo(frame.x, caret.y);

   _doc->_text->validateBookmark(bm);
   row = bm.getRow();
}

bool Document::Reader ::readCurrentLine(_ELENA_::TextWriter& writer, size_t length)
{
   style = 0;
   bandStyle = false;

   _doc->_text->validateBookmark(bm);

   int column = _region.bottomRight.x;
   if ((bm.getColumn() < column) && !bm.isEOL()) {
      size_t styleLen = _doc->defineStyle(*this);
      if (styleLen < length)
         length = styleLen;

      _doc->_text->copyLineToX(bm, writer, length, column);

      return true;
   }
   else return false;
}

bool Document::Reader :: readNext(_ELENA_::TextWriter& writer, size_t length)
{
   style = 0;
   bandStyle = false;

   if (bm.isEOF())
     return false;

   if (bm.getColumn() >= _region.bottomRight.x || bm.isEOL()) {
      if (bm.getRow() >= _region.bottomRight.y)
         return false;

      bm.moveTo(_region.topLeft.x, bm.getRow() + 1);
      newLine = true;
      row = bm.getRow();
   }
   size_t styleLen = _doc->defineStyle(*this);
   if (styleLen < length)
      length = styleLen;

   _doc->_text->copyLineTo(bm, writer, length, true);

   return true;
}

// --- Document ---

Document :: Document(Text* text, LexicalStyler* styler, int encoding)
   : _undoBuffer(UNDO_BUFFER_SIZE)
{
   _text = text;
   _styler = styler;

   _encoding = encoding;
   _selection = 0;
   _maxColumn = 0;

   _text->attachWatcher(this);

   status.init();
}

Document :: ~Document()
{
   _ELENA_::freeobj(_styler);

   _text->detachWatcher(this);
}

disp_t Document :: getSelectionLength()
{
   return abs(_selection);
}

void Document :: resize(Point size)
{
   _size = size;

   setCaret(_caret.getCaret(), false);
}

void Document :: moveLeft(bool selecting)
{
   if (selecting) {
      size_t pos = _caret.getPosition();
      _caret.moveOn(-1);

      _selection += pos - _caret.getPosition();
   }
   else _caret.moveOn(-1);

   setCaret(_caret.getCaret(), selecting);
}

void Document :: moveRight(bool selecting)
{
   if (selecting) {
      size_t pos = _caret.getPosition();
	  _caret.moveOn(1);

	  _selection += pos - _caret.getPosition();
   }
   else _caret.moveOn(1);

   setCaret(_caret.getCaret(), selecting);
}

void Document :: moveUp(bool selecting)
{
   if (_caret.getRow() > 0) {
      setCaret(_caret.getColumn(), _caret.getRow() - 1, selecting);
   }
}

void Document :: moveDown(bool selecting)
{
   if (_caret.getRow() < _text->getRowCount()) {
      setCaret(_caret.getColumn(), _caret.getRow() + 1, selecting);
   }
}

void Document :: moveHome(bool selecting)
{
   setCaret(0, _caret.getRow(), selecting);
}

void Document :: moveEnd(bool selecting)
{
   setCaret((int)_caret.getLength(), _caret.getRow(), selecting);
}

void Document :: moveFirst(bool selecting)
{
   return setCaret(0, 0, selecting);
}

void Document :: moveLast(bool selecting)
{
   int lastRow = _text->getRowCount() - 1;
   return setCaret((int)_text->getRowLength(lastRow), lastRow, selecting);
}

void Document :: moveLeftToken(bool selecting)
{
   bool newToken = false;
   bool _operator = false;
   size_t pos = _caret.getPosition();

   _caret.moveOn(-1);

   while (_caret.getColumn() > 0 || _caret.getRow() > 0) {
      if (_caret.getColumn() == _caret.getLength())
         break;

      size_t length;
      text_t line = _text->getLine(_caret, length);

      if (text_str(WHITESPACE).find(line[0])!=-1) {
         if (_operator || newToken) {
            _caret.moveOn(1);
            break;
         }
      }
      else if (text_str(OPERATORS).find(line[0])!=-1) {
         if (newToken) {
            _caret.moveOn(1);
            break;
         }
         else _operator = true;
      }
      else {
         if (_operator) {
            _caret.moveOn(1);
            break;
         }
         else newToken = true;
      }
      _caret.moveOn(-1);
   }
   if (selecting) {
      _selection += pos - _caret.getPosition();
   }
   setCaret(_caret.getCaret(), selecting);
}

void Document :: moveRightToken(bool selecting, bool trimWhitespace)
{
   bool newToken = false;
   bool _operator = false;
   bool first = true;
   size_t pos = _caret.getPosition();
   while (first || _caret.getColumn() < (int)_caret.getLength()) {
      size_t length;
      text_t line = _text->getLine(_caret, length);
      if (length == 0)
         break;

      if (text_str(WHITESPACE).find(line[0])!=-1) {
         newToken = true;
         _operator = false;

         if (trimWhitespace)
            break;
      }
      else if (text_str(OPERATORS).find(line[0])!=-1) {
         if (!_operator && !first) {
            break;
         }
         _operator = true;
      }
	   else if (newToken || _operator)
         break;

      _caret.moveOn(1);
      first = false;
   }
   if (selecting) {
      _selection += pos - _caret.getPosition();
   }
   setCaret(_caret.getCaret(), selecting);
}


void Document :: moveFrameUp()
{
   vscroll(-1);
   if (status.frameChanged) {
      if (_frame.getRow() + _size.y - 2 <= _caret.getRow()) {
         setCaret(_caret.getColumn(), _frame.getRow() + _size.y - 3, false);
      }
   }
}

void Document :: moveFrameDown()
{
   vscroll(1);
   if (status.frameChanged) {
      if (_caret.getRow() < _frame.getRow()) {
         setCaret(_caret.getColumn(), _frame.getRow(), false);
      }
   }
}

void Document :: movePageUp(bool selecting)
{
   if (_caret.getRow()==0)
      return;

   if (_frame.getRow()==0) {
      setCaret(_caret.getColumn(), 0, selecting);
   }
   else {
      vscroll(-_size.y);
      if (status.frameChanged)
         setCaret(_caret.getColumn(), _caret.getRow() - _size.y, selecting);
   }
}

void Document :: movePageDown(bool selecting)
{
   if (_caret.getRow() + _size.y > _text->getRowCount() - 1) {
      setCaret(_caret.getColumn(), _text->getRowCount() - 1, selecting);
   }
   else {
      vscroll(_size.y);
      if (status.frameChanged)
         setCaret(_caret.getColumn(), _caret.getRow() + _size.y, selecting);
   }
}

void Document :: selectWord()
{
   moveLeftToken(false);
   moveRightToken(true, true);
}

void Document :: setCaret(int column, int row, bool selecting)
{
   if (column < 0) column = 0;
   if (row < 0) row = 0;
   else if (row >= _text->getRowCount()) row = _text->getRowCount() - 1;

   _text->validateBookmark(_caret);

   size_t position = _caret.getPosition();

   _caret.moveTo(column, row);
   if (_maxColumn < (int)_caret.getLength() + _size.x) {
      _maxColumn = (int)_caret.getLength() + _size.x;

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

   if (_frame.getCaret()!=frame) {
      _text->validateBookmark(_frame);
      _frame.moveTo(frame.x, frame.y);
      status.frameChanged = true;
   }
   if (selecting) {
      _selection += position - _caret.getPosition();
      status.selelectionChanged = true;
   }
   else {
      if (_selection != 0)
         status.selelectionChanged = true;

      _selection = 0;
   }

   status.caretChanged = true;
}

void Document :: setCaret(HighlightInfo info, bool selecting)
{
   if (info.col != 0) {
      setCaret(info.col, info.row, selecting);
   }
   else setCaret(retrieveColumn(info.row, info.disp), info.row, selecting);
}

int Document :: retrieveColumn(int row, int disp)
{
   TextBookmark bm = _caret;

   _text->validateBookmark(bm);

   bm.moveTo(0, row);
   bm.moveOn(disp);

   return bm.getColumn();
}

void Document :: vscroll(int displacement)
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

void Document :: hscroll(int displacement)
{
   Point frame = _frame.getCaret();

   frame.x += displacement;
   if (frame.x < 0)
      frame.x = 0;

   if (_frame.getCaret() != frame) {
      _frame.moveTo(frame.x, frame.y);
      status.frameChanged = true;
   }
}

bool Document :: eraseSelection()
{
   if (_selection==0)
      return false;

   _text->validateBookmark(_caret);

   if (_selection < 0) {
      _caret.moveOn(_selection);
      _selection = -_selection;
   }
   _text->eraseLine(_caret, _selection);
   _selection = 0;
   status.selelectionChanged = true;

   return true;
}

void Document :: insertChar(text_c ch, size_t count)
{
   if (hasSelection()) {
      int rowCount = _text->getRowCount();

      eraseSelection();

      status.rowDifference += (_text->getRowCount() - rowCount);
   }
   else if (status.overwriteMode && (int)_caret.getLength() > _caret.getColumn(false)) {
      _text->eraseChar(_caret);
   }

   while (count > 0) {
      if (_text->insertChar(_caret, ch)) {
         _text->validateBookmark(_caret);
         _caret.moveOn(1);
         setCaret(_caret.getCaret(), false);
      }
      else break;

      count--;
   }
}

void Document :: insertLine(text_t text, disp_t length)
{
   int rowCount = _text->getRowCount();

   eraseSelection();

   _text->insertLine(_caret, text, length);
   _caret.moveOn(length);

   setCaret(_caret.getCaret(), false);

   status.rowDifference += (_text->getRowCount() - rowCount);
}

void Document :: insertNewLine()
{
   int rowCount = _text->getRowCount();

   eraseSelection();

   if (_text->insertNewLine(_caret)) {
      setCaret(0, _caret.getRow() + 1, false);
   }

   status.rowDifference += (_text->getRowCount() - rowCount);
}

void Document :: eraseChar(bool moveback)
{
   int rowCount = _text->getRowCount();

   if (_selection != 0) {
      eraseSelection();

      setCaret(_caret.getCaret(), false);
   }
   else {
      if (moveback) {
         if (_caret.getColumn(false)==0 && _caret.getRow()==0)
            return;

         moveLeft(false);
      }
      _text->eraseChar(_caret);
   }

   status.rowDifference += (_text->getRowCount() - rowCount);
}

void Document :: onUpdate(size_t position)
{
}

void Document :: onInsert(size_t position, size_t length, text_t line)
{
   _undoBuffer.onInsert(position, length, line);

   _frame.invalidate();

   if (_caret.getPosition() > position) {
      _caret.invalidate();
   }
   status.modifiedMode = true;
   status.frameChanged = true;
}

void Document :: onErase(size_t position, size_t length, text_t line)
{
   _undoBuffer.onErase(position, length, line);

   _frame.invalidate();

   if (_caret.getPosition() > position) {
      _caret.invalidate();
   }
   status.modifiedMode = true;
   status.frameChanged = true;
}

size_t Document :: defineStyle(Reader& reader)
{
   size_t length = _size.x;
   size_t position = reader.bm.getPosition();

   if (_selection != 0) {
      size_t curPos = _caret.getPosition();
      size_t selPos = curPos + _selection;
      if (_selection > 0) {
         if (reader.bm.getRow()==_caret.getRow() && position < curPos) {
            length = min(curPos - position, length);
         }
         else if (position >= curPos && position < selPos) {
            reader.style = STYLE_SELECTION;

            return min(selPos - position, length);
         }
      }
      else {
         if (position >= selPos && position < curPos) {
            reader.style = STYLE_SELECTION;

            return min(curPos - position, length);
         }
         else if (position < selPos && position + length > selPos) {
            length = min(selPos - position, length);
         }
      }
   }
   if (_styler) {
      _ELENA_::pos_t proceeded = _styler->proceed(position, reader);
      return min(length, proceeded);
   }
   else return length;
}

void Document :: copySelection(text_c* text)
{
   if (_selection==0)  {
      text[0] = 0;
   }
   else {
      _text->copyTo(_caret, text, _selection);
   }
}

void Document :: copyText(text_c* text, int length)
{
   if (length==0)  {
      text[0] = 0;
   }
   else {
      _text->copyTo(_caret, text, length);
   }
}

void Document :: tabbing(text_c space, size_t count, bool indent)
{
   _text->validateBookmark(_caret);

   if (_selection < 0) {
      _caret.moveOn(_selection);
      _selection = abs(_selection);
   }
   TextBookmark end = _caret;

   end.moveOn(_selection);

   // if only part of the line was selected just insert tab
   int lastRow = end.getRow();
   if (lastRow == _caret.getRow()) {
      if (indent)
         insertChar(space, count);
   }
   else {
      setCaret(0, _caret.getRow(), true);

      if (end.getColumn() == 0)
         lastRow--;

      TextBookmark start = _caret;
      while (start.getRow() <= lastRow) {
         if (indent) {
            for (size_t i = 0 ; i < count ; i++) {
               _text->insertChar(start, space);
               _selection++;
            }
         }
         else {
            size_t length;
            for (size_t i = 0 ; i < count ; i++) {
               text_t s = _text->getLine(start, length);
               if (length!=0 && (s[0]==' ' || s[0]=='\t')) {
                  bool tab = (s[0]=='\t');

                  _text->eraseChar(start);
                  _selection--;

                  if (tab)
                     break;
               }
               else break;
            }
         }
         if(!start.moveTo(0, start.getRow() + 1))
            break;
      }
   }
}

bool Document :: canUndo()
{
   return !_undoBuffer.Bof();
}

bool Document :: canRedo()
{
   return !_undoBuffer.Eof();
}

void Document :: undo()
{
   _undoBuffer.undo(_text, _caret);

   setCaret(_caret.getCaret(false), false);
}

void Document :: redo()
{
   _undoBuffer.redo(_text, _caret);

   setCaret(_caret.getCaret(false), false);
}

void Document :: save(_ELENA_::path_t path)
{
   _text->save(path, _encoding);

   status.modifiedMode = false;
   status.unnamed = false;
}

void Document :: moveToFrame(int column, int row, bool selecting)
{
   setCaret(_frame.getColumn() + column, _frame.getRow() + row, selecting);
}

void Document :: trim()
{
   int rowCount = _text->getRowCount();

   Point caret = _caret.getCaret(false);
   bool space = false;
   if ((size_t)caret.x == _caret.getLength()) {
      _text->eraseChar(_caret);
   }
   else while ((size_t)caret.x < _caret.getLength()) {
      size_t length;
      text_t line = _text->getLine(_caret, length);

      if (line[0]==' ' || line[0]=='\t') {
         _text->eraseChar(_caret);
         space = true;
      }
      else if (!space) {
         _text->eraseChar(_caret);
      }
      else break;
   }

   status.rowDifference += (_text->getRowCount() - rowCount);
}

void Document :: duplicateLine()
{
   Point caret = _caret.getCaret(false);

   _caret.moveTo(0, caret.y);

   TextBookmark bm = _caret;
   bm.moveTo((int)_caret.getLength(), caret.y);

   size_t length = bm.getPosition() - _caret.getPosition();
   text_c* buffer = _ELENA_::StrFactory::allocate(length + 1, DEFAULT_TEXT);
   _text->copyTo(_caret, buffer, length);

   _caret.moveTo(0, caret.y + 1);
   _text->insertNewLine(_caret);
   _text->insertLine(_caret, buffer, length);

   _ELENA_::freestr(buffer);

   setCaret(caret.x, caret.y + 1, false);
}

void Document :: eraseLine()
{
   _caret.moveTo(0, _caret.getRow());

   _text->eraseLine(_caret, _caret.getLength());
   _text->eraseChar(_caret);
}
void Document :: commentBlock()
{
   disp_t selection = _selection;
   _selection = 0;

   if (selection < 0) {
      _caret.moveOn(selection);
      selection = -selection;
   }
   Point caret = _caret.getCaret();
   TextBookmark end = _caret;
   end.moveOn(selection);

   while (caret.y < end.getRow() || (caret.y == end.getRow() && end.getColumn() > 0)) {
      if (!_caret.moveTo(0, caret.y))
         return;

      insertLine(_T("//"), 2);
      caret.y++;
   }
}

void Document :: uncommentBlock()
{
   disp_t selection = _selection;
   _selection = 0;

   if (selection < 0) {
      _caret.moveOn(selection);
      selection = -selection;
   }
   Point caret = _caret.getCaret();
   TextBookmark end = _caret;
   end.moveOn(selection);

   text_c line[3];
   while (caret.y <= end.getRow()) {
      if (!_caret.moveTo(0, caret.y))
         return;

      _text->copyTo(_caret, line, 2);
      if (text_str(_T("//")).compare(line, 2)) {
         eraseChar(false);
         eraseChar(false);
      }
      caret.y++;
   }
}

void Document :: toUppercase()
{
   disp_t selection = getSelectionLength();
   if (selection > 0) {
      text_c* buffer = _ELENA_::StrFactory::allocate(selection + 1, DEFAULT_TEXT);
      copySelection(buffer);

      _ELENA_::StrHelper::upper(buffer);

      insertLine(buffer, selection);

      _ELENA_::freestr(buffer);
   }
   else {
      text_c buffer[2];
      copyText(buffer, 1);

      if (text_str(WHITESPACE).find(buffer[0])==-1) {
         _ELENA_::StrHelper::upper(buffer);

         eraseChar(false);
         insertChar(buffer[0], 1);
      }
   }
}

void Document :: toLowercase()
{
   disp_t selection = getSelectionLength();
   if (selection > 0) {
      text_c* buffer = _ELENA_::StrFactory::allocate(selection + 1, DEFAULT_TEXT);
      copySelection(buffer);

      _ELENA_::StrHelper::lower(buffer);

      insertLine(buffer, selection);

      _ELENA_::freestr(buffer);
   }
   else {
      text_c buffer[2];
      copyText(buffer, 1);

      if (text_str(WHITESPACE).find(buffer[0])==-1) {
         _ELENA_::StrHelper::lower(buffer);

         eraseChar(false);
         insertChar(buffer[0], 1);
      }
   }
}
void Document :: swap()
{
   if (_caret.getColumn() > 0 && _caret.getColumn() < (int)_caret.getLength()) {
      text_c pair[3];

      _caret.moveOn(-1);
      _selection = 2;

      copySelection(pair);

      // swap
      text_c tmp = pair[0];
      pair[0] = pair[1];
      pair[1] = tmp;

      insertLine(pair, 2);

      _selection = 0;
   }
}

bool Document :: findLine(text_t text, bool matchCase, bool wholeWord)
{
   TextBookmark bookmark = _caret;
   if (_text->findWord(bookmark, text, matchCase, wholeWord ? TERMINATORS : NULL)) {
      setCaret(bookmark.getCaret(false), false);
      setCaret(_caret.getColumn() + (int)_ELENA_::getlength(text), _caret.getRow(), true);

      return true;
   }
   else return false;
}
