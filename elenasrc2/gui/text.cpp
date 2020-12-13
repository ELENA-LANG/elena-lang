//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Text class implementation
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "text.h"

using namespace _GUI_;
using namespace _ELENA_;

#ifdef _WIN64

#define ERASE_MODE 0x8000000000000000

inline size_t readData(MemoryDump* buffer, size_t offset)
{
   size_t size;
   buffer->read(offset, &size, sizeof(size_t));

   return size;
}

inline bool testEraseMode(size_t value)
{
   return testLong(value, ERASE_MODE);
}

inline void trimData(MemoryDump* buffer, size_t offset)
{
   buffer->trimLong(offset);
}

#else

#define ERASE_MODE 0x80000000

inline size_t readData(MemoryDump* buffer, size_t offset)
{
   size_t size;
   buffer->read(offset, &size, sizeof(size_t));

   return size;
}

inline bool testEraseMode(size_t value)
{
   return test(value, ERASE_MODE);
}

inline void trimData(MemoryDump* buffer, size_t offset)
{
   buffer->trim(offset);
}

#endif

// --- Text static variables ---
int Text::TabSize = 4;

// --- TextBookmark ---

TextBookmark :: TextBookmark()
{
   _status = BM_INVALID;
   _row = _virtual_column = _column = 0;
   _length = BM_INVALID;
}

TextBookmark :: ~TextBookmark()
{
}

void TextBookmark :: set(Pages* pages)
{
   // save the current position
   Point target = getCaret();

   // reset bookmark to the beginning
   _status = 0;
   _position = 0;
   _offset = 0;
   _page = pages->start();

   _row = _column = 0;
   _length = BM_INVALID;

   // go to the current position
   moveTo(target.x, target.y);
}

bool TextBookmark :: goToPreviousPage(bool allowEmpty)
{
   do {
      if (!_page.First()) {
         _page--;
         _position -= (*_page).used;
         _offset = (*_page).used - 1;
      }
      else {
         _offset = _position = 0;
         refreshStatus();
         return false;
      }

   } while (!allowEmpty && (*_page).used == 0);

   if ((*_page).used == 0)
      _offset = 0;

   refreshStatus();
   return true;
}

bool TextBookmark :: goToNextPage(bool allowEmpty)
{
   do {
      if (!_page.Last()) {
         _position += (*_page).used;
         _offset = 0;
         _page++;
      }
      else {
         _status = BM_EOT;
         _offset = (*_page).used;

         return false;
      }
   } while (!allowEmpty && (*_page).used == 0);

   refreshStatus();

   return true;
}

//bool TextBookmark :: skipEmptyPages(bool onlyForward)
//{
//   if ((*_page).used==0) {
//      if (!goToNextPage()) {
//         if (!onlyForward) {
//            return goToPreviousPage();
//         }
//         else return false;
//      }
//   }
//   return true;
//}

void TextBookmark :: normalize()
{
   refreshStatus();

   if(_status == BM_EOP)
      goToNextPage();
}

bool TextBookmark :: go(disp_t disp, bool allowEmpty)
{
   if (disp < 0) {
      while (_offset < (size_t)-disp) {
         disp += _offset + 1;
         if (!goToPreviousPage(allowEmpty))
            return false;
      }
      _offset += disp;
   }
   else {
      while ((*_page).used < _offset + disp) {
         disp -= ((*_page).used - _offset);
         if (!goToNextPage(allowEmpty))
            return false;
      }
      _offset += disp;
   }
   refreshStatus();
   if (!allowEmpty && _status == BM_EOP) {
      goToNextPage();
   }
   return (_status != BM_EOT);
}

bool TextBookmark :: move(disp_t disp)
{
   if (disp < 0) {
      // go to the next page start if <end of the page>
      if(_status == BM_EOP)
         goToNextPage();

      while (disp < 0) {
         disp++;
         if (!prevChar(disp))
            return false;

         if ((*_page).text[_offset]==10) {
            _length = BM_INVALID;
            _row--;

            TextBookmark bm = *this;

            // if it is a CRLF - skip LF
            go(-1);
            if ((*_page).text[_offset]==13) {
               disp++;
            }
            else {
               go(1);
               bm.go(1); // HOTFIX : to properly define the current line length
            }

            bm.moveToPrevBOL();

            _length = bm.getLength();
            _column = (int)_length;  // !! temporal
         }
         else if ((*_page).text[_offset] == 13) {
            //HOTFIX : to deal with \r\r
            _length = BM_INVALID;
            _row--;

            TextBookmark bm = *this;

            bm.moveToPrevBOL();

            _length = bm.getLength();
            _column = (int)_length;  // !! temporal
         }
         else if ((*_page).text[_offset]==0x09) {
            TextBookmark bm = *this;
            if (!bm.moveToPrevBOL())
               bm.moveToStart();

            bm.move(getPosition() - bm.getPosition());

            _column = bm._column;
         }
         else _column--;
      }
      return true;
   }
   else if (disp > 0) {
      // go to the next page start if <end of the page>
      if(_status == BM_EOT || (_status == BM_EOP && !goToNextPage()))
         return false;

      bool valid = true;
      while (valid && disp > 0) {
         disp--;

         if ((*_page).text[_offset]==13) {
            _row++;
            _column = 0;
            _length = BM_INVALID;

            if(!go(1))
               return false;

            if ((*_page).text[_offset]==10) {
               valid = go(1);

               disp--;
            }
         }
         else if ((*_page).text[_offset]==10) {
            _row++;
            _column = 0;
            _length = BM_INVALID;

            valid = go(1);
         }
         else if ((*_page).text[_offset]==0x09) {
            _column += _ELENA_::calcTabShift(_column, Text::TabSize);

            valid = go(1);
         }
         else {
            valid = nextChar(disp);
            _column++;
         }
      }
      return valid;
   }
   else return true;
}

void TextBookmark :: moveToStart()
{
   //if (_page.First() && _position == 0 && _offset == 0)
   //   return false;

   // go to the text start
   while (goToPreviousPage());

   _column = 0;
   _row = 0;
   _length = BM_INVALID;

//   skipEmptyPages(true);
}

void TextBookmark :: moveToClosestRow(int row)
{
   moveToStart();
   if (row != _row) {
      int closestRow = 0;
      // looking for the closest page
      while (closestRow + (*_page).rows < row) {
         closestRow += (*_page).rows;
         if (!goToNextPage()) {
            _row = closestRow;
            return;
         }
      }
      _offset = 0;

      // looking for the closest row
      _row = closestRow;
      moveToNextBOL();
   }
}

bool TextBookmark :: moveToClosestPosition(size_t position)
{
   moveToStart();
   if (position != 0) {
      while (getPosition() + (*_page).used < position) {
         _row += (*_page).rows;
         if (!goToNextPage())
            return false;
      }
      moveToNextBOL();
      return move(position - getPosition());
   }
   return true;
}

void TextBookmark :: moveToClosestColumn(int column)
{
   if (column > _column) {
      while (column > _column && move(1));
   }
   else if (column < _column) {
      while (column < _column && move(-1));
   }
}

bool TextBookmark :: moveToPrevBOL()
{
   _column = 0;
   _length = BM_INVALID;

   if (_row==0) {
      moveToStart();
      return false;
   }
   // find EOL
   if (!go(-1))
      return false;

   if ((*_page).text[_offset]==10) {
      _row--;
      if (!go(-1))
         return false;
   }
   // move until BOL
   while ((*_page).text[_offset]!=10) {
      if (!go(-1))
         return false;
   }
   go(1);

   return true;
}

bool TextBookmark :: moveToNextBOL()
{
   if (_status == BM_EOT)
      return false;

   // go to the next page start if <end of the page>
   if(_status == BM_EOT || (_status == BM_EOP && !goToNextPage()))
      return false;

   // move until EOL
   while ((*_page).text[_offset]!=10) {
      if (!go(1))
         return false;
   }
   // go to the next position
   go(1);
   _column = 0;
   _length = BM_INVALID;
   _row++;

   return true;
}

size_t TextBookmark :: seekEOL()
{
   TextBookmark bm = *this;

   // go to the next page start if <end of the page>
   if(bm._status == BM_EOT || (bm._status == BM_EOP && !bm.goToNextPage()))
      return 0;

   text_c ch = (*bm._page).text[bm._offset];
   while (ch != 10 && ch != 13) {
      if (!bm.move(1))
         break;

      ch = (*bm._page).text[bm._offset];
   }

   return bm._column - _column;
}

bool TextBookmark :: moveTo(int column, int row)
{
   if (_status==BM_INVALID)
      return false;

   refreshStatus();

   if (column == 0 && row == 0)
      moveToStart();

   // backward navigation
   if (row < _row) {
      // estimate if backward navigation is feasible (two times less than number of rows on the page)
      if ((_row - row) < ((*_page).rows << 1)) {
         while (_row > row && moveToPrevBOL());
      }
	   else moveToClosestRow(row);
   }
   // estimate if forward navigation is feasible (two times less than number of rows on the page)
   if (((row - _row) > 2) && (row - _row) > ((*_page).rows << 1)) {
      moveToClosestRow(row);
   }
   while (row > _row && moveToNextBOL());

   if (row != _row)
      return false;

   _virtual_column = column;
   if ((size_t)column > getLength())
      column = (int)getLength();

   moveToClosestColumn(column);

   return true;
}

bool TextBookmark :: moveOn(disp_t disp)
{
   if ((size_t)_status==BM_INVALID)
      return false;

   refreshStatus();

   bool valid = false;
   if (disp < 0) {
      disp = abs(disp);
      if (disp < (PAGE_SIZE << 2)) {
         valid = move(-disp);
      }
      else valid = moveToClosestPosition(getPosition() - disp);
   }
   else if (disp > 0) {
      if (disp < (PAGE_SIZE << 2)) {
         valid = move(disp);
      }
      else valid = moveToClosestPosition(getPosition() + disp);
   }
   _virtual_column = _column;
   return valid;
}

size_t TextBookmark :: getLength()
{
   if (_length==BM_INVALID) {
      _length = _column + seekEOL();
   }
   return _length;
}

// --- TextScanner ---

TextScanner :: TextScanner(Text* text)
{
   _text = text;
   _text->validateBookmark(_bookmark);
}

text_t TextScanner :: getLine(size_t& length)
{
   return _text->getLine(_bookmark, length);
}

// --- Text ---

Text :: Text(EOLMode mode)
{
   _mode = mode;

   _rowCount = 0;
}

Text :: ~Text()
{
}

void Text :: validateBookmark(TextBookmark& bookmark)
{
   if (!bookmark.isValid()) {
      bookmark._mode = _mode;
      bookmark.set(&_pages);
   }
}

void Text :: create()
{
   _pages.clear();
   _pages.add(Page());

   _rowCount = 0;
}

bool Text :: load(_ELENA_::path_t path, int encoding, bool autoDetecting)
{
   _ELENA_::FileReader file(path, encoding, autoDetecting);
   encoding = file.getEncoding();

   if (!file.isOpened())
      return false;

   while (!file.Eof()) {
      Page page;
      file.readText(page.text, PAGE_SIZE, page.used);

      _pages.add(page);
   }
   if (_pages.Count()==0) {
      create();
   }
   else _rowCount = 1;

   Pages::Iterator it = _pages.start();
   while (!it.Eof()) {
      refreshPage(it);
	  _rowCount += (*it).rows;

      it++;
   }
   return true;
}

void Text :: save(_ELENA_::path_t path, int encoding)
{
   _ELENA_::FileWriter writer(path, encoding, false);

   Pages::Iterator it = _pages.start();
   while (!it.Eof()) {
      writer.writeText((*it).text, (*it).used);

      it++;
   }
}

void Text :: refreshPage(Pages::Iterator page)
{
   size_t used = (*page).used;
   text_c* text = (*page).text;

   (*page).rows = 0;
   for (size_t i = 0 ; i < used ; i++) {
      if (text[i]==10) {
         (*page).rows++;
      }
   }
}

void Text :: refreshNextPage(Pages::Iterator page)
{
   if (!page.Last()) {
      page++;
      refreshPage(page);
   }
}

int Text :: retrieveRowCount()
{
   Pages::Iterator it = _pages.start();
   int count = 1;
   while (!it.Eof()) {
      count += (*it).rows;

      it++;
   }
   return count;
}

size_t Text :: getRowLength(int row)
{
   if (row < _rowCount) {
      TextBookmark bookmark;
      validateBookmark(bookmark);

      bookmark.moveTo(0, row);

      return bookmark.getLength();
   }
   else return 0;
}

void Text :: copyLineToX(TextBookmark& bookmark, _ELENA_::TextWriter& writer, size_t length, int x)
{
   validateBookmark(bookmark);

   bookmark.normalize();
   if (bookmark._status == BM_EOT)
      return;

   int diff = bookmark.getVirtualDiff();
   if (diff > 0) {
      writer.fillText(_T(" "), 1, diff);
      diff = 0;
   }

   int col = bookmark._column;
   while (length > 0 && col < x) {
      size_t offset = bookmark._offset;
      size_t count = (*bookmark._page).used - offset;
      if (count > length) {
         count = length;
      }
      text_t line = (*bookmark._page).text + offset;
      size_t i = 0;
      while (i < count && col < x) {
         if (line[i] == 13 || line[i] == 10) {
            bookmark.moveOn(i);
            return;
         }
         else if (line[i] == '\t') {
            int disp = _ELENA_::calcTabShift(col, Text::TabSize);
            writer.fillText(_T(" "), 1, disp + diff);
            diff = 0;
            col += disp;
            i++;
         }
         else {
            size_t chLen = TextBookmark::charLength(line, i);
            if (chLen > 1) {
               if (i + chLen < count) {
                  writer.write(&line[i], chLen);
                  i += (chLen - 1);
               }
               else break;
            }
            else writer.write(&line[i], 1u);

            col++;
            i++;
         }
      }

      if (!bookmark.moveOn(i))
         break;

      length -= i;
   }
}

void Text :: copyLineTo(TextBookmark& bookmark, _ELENA_::TextWriter& writer, size_t length, bool stopOnEOL)
{
   validateBookmark(bookmark);

   bookmark.normalize();
   if (bookmark._status == BM_EOT)
      return;

   int diff = bookmark.getVirtualDiff();
   if (diff > 0) {
      writer.fillText(_T(" "), 1, diff);
      diff = 0;
   }

   int col = bookmark._column;
   while (length > 0) {
      size_t offset = bookmark._offset;
      size_t count = (*bookmark._page).used - offset;
      if (count > length) {
         count = length;
      }
      text_t line = (*bookmark._page).text + offset;
      size_t i = 0;
      while (i < count) {
         if (stopOnEOL && (line[i] == 13 || line[i] == 10)) {
            bookmark.moveOn(i);
            return;
         }
         else if (line[i] == '\t') {
            int disp = _ELENA_::calcTabShift(col, Text::TabSize);
            writer.fillText(_T(" "), 1, disp + diff);
            diff = 0;
            col += disp;
            i++;
         }
         else {
            size_t chLen = TextBookmark::charLength(line, i);
            if (chLen > 1) {
               if (i + chLen < count) {
                  writer.write(&line[i], chLen);
                  i += (chLen - 1);
               }
               else break;
            }
            else writer.write(&line[i], 1u);

            col++;
            i++;
         }
      }

      if (!bookmark.moveOn(i))
         break;

      length -= i;
   }
}

void Text :: copyTo(TextBookmark bookmark, text_c* buffer, disp_t length)
{
   validateBookmark(bookmark);

   if (length < 0) {
      bookmark.go(length);
      length = -length;
   }

   buffer[length] = 0;
   while (length > 0) {
      if (bookmark._offset >= (int)(*bookmark._page).used) {
         bookmark.goToNextPage();
      }
      size_t copied = (*bookmark._page).used - bookmark._offset;
      if (copied > (size_t)length) {
         copied = length;
      }
      _ELENA_::StrHelper::move(buffer, (*bookmark._page).text + bookmark._offset, copied);

      if (!copied)
         break;

      bookmark.go(copied);
      buffer += copied;
      length -= copied;
   }
}

text_t Text :: getLine(TextBookmark& bookmark, size_t& length)
{
   validateBookmark(bookmark);

   bookmark.normalize();
   if (bookmark._status == BM_EOT) {
      length = 0;
      return NULL;
   }
   else {
      length = (*bookmark._page).used - bookmark._offset;

      return (*bookmark._page).text + bookmark._offset;
   }
}

text_c Text :: getChar(TextBookmark& bookmark)
{
   validateBookmark(bookmark);

   bookmark.normalize();
   if (bookmark._status == BM_EOT) {
      return 0;
   }
   else return *((*bookmark._page).text + bookmark._offset);
}

void Text :: insert(TextBookmark bookmark, text_t s, size_t length, bool checkRowCount)
{
   size_t position = bookmark.getPosition();

   TextWatchers::Iterator it = _watchers.start();
   while (!it.Eof()) {
      (*it)->onInsert(position, length, s);
      it++;
   }
   size_t offset = bookmark._offset;
   size_t size;
   while (length > 0) {
      Pages::Iterator page = bookmark._page;
      size = PAGE_SIZE - (*page).used;
      if (size > length)
         size = length;

      if (offset < (*page).used) {
         if (size==0) {
            size = (*page).used - offset;
            (*page).used = offset;

            Page newPage(size);
            _ELENA_::StrHelper::move(newPage.text, (*page).text + offset, size);

            _pages.insertAfter(page, newPage);

            if (!checkRowCount) {
               refreshPage(page);
            }
            refreshNextPage(page);

            if (size > length)
               size = length;
         }
         else _ELENA_::StrHelper::move((*page).text + offset + size, (*page).text + offset, (*page).used - offset);
      }
      else if (size==0) {
         if (!bookmark.goToNextPage(true)) {
            Page newPage;

            _pages.insertAfter(bookmark._page, newPage);
            bookmark.goToNextPage(true);
         }
         offset = bookmark._offset;
         continue;
      }
      _ELENA_::StrHelper::move((*page).text + offset, s, size);

      (*page).used += size;
      if (checkRowCount) {
         refreshPage(page);
      }
      length -= size;
      offset += size;
      s += size;
   }

   it = _watchers.start();
   while (!it.Eof()) {
      (*it)->onUpdate(position);
      it++;
   }
}

void Text :: erase(TextBookmark bookmark, size_t length, bool checkRowCount)
{
   size_t size = 0;
   size_t offset = bookmark._offset;
   size_t position = bookmark.getPosition();

   while (length > 0) {
      Pages::Iterator page = bookmark._page;
      size = length;
      if (size > (*page).used - offset)
         size = (*page).used - offset;

      if (size != 0) {
         TextWatchers::Iterator it = _watchers.start();
         while (!it.Eof()) {
            (*it)->onErase(position, size, (*page).text + offset);
            it++;
         }

         if (offset + size < (*page).used) {
            size_t l = (*page).used - offset;
            _ELENA_::StrHelper::move((*page).text + offset, (*page).text + offset + size, l);
         }
         (*page).used -= size;
         length -= size;
         if (checkRowCount) {
            refreshPage(page);
         }
      }
      if (length != 0) {
         if (!bookmark.goToNextPage())
            break;

         offset = bookmark._offset;
         position = bookmark.getPosition();
      }
   }
   TextWatchers::Iterator it = _watchers.start();
   while (!it.Eof()) {
      (*it)->onUpdate(position);
      it++;
   }
}

bool Text :: insertChar(TextBookmark& bookmark, text_c ch)
{
   validateBookmark(bookmark);

   insert(bookmark, &ch, 1, false);
   if (ch=='\t') {
      bookmark._length = BM_INVALID;
   }
   else if (bookmark._length != BM_INVALID)
      bookmark._length++;

   if (_rowCount==0) {
      _rowCount++;
   }
   return true;
}

bool Text :: insertLine(TextBookmark& bookmark, text_t s, size_t length)
{
   validateBookmark(bookmark);

   insert(bookmark, s, length, true);
   bookmark._length = BM_INVALID;

   _rowCount = retrieveRowCount();

   return true;
}

bool Text :: insertNewLine(TextBookmark& bookmark)
{
   validateBookmark(bookmark);

   text_c ch[2];
   if (_mode == eolCRLF) {
      ch[0] = 13;
      ch[1] = 10;
      insert(bookmark, ch, 2, true);
   }
   else {
      ch[0] = 10;
      insert(bookmark, ch, 1, true);
   }

   bookmark._length = BM_INVALID;

   _rowCount++;

   return true;
}

bool Text :: eraseChar(TextBookmark& bookmark)
{
   validateBookmark(bookmark);

   if (bookmark._column==bookmark.getLength()) {
      if (bookmark._row != _rowCount - 1) {
         if ((*bookmark._page).text[bookmark._offset] == 13) {
            erase(bookmark, 2, true);
         }
         else erase(bookmark, 1, true);

         _rowCount--;
      }
      else return false;
   }
   else erase(bookmark, 1, false);

   bookmark._length = BM_INVALID;
//   bookmark.skipEmptyPages();

   return true;
}

bool Text :: eraseLine(TextBookmark& bookmark, size_t length)
{
   validateBookmark(bookmark);

   erase(bookmark, length, true);
   bookmark._length = BM_INVALID;
//   bookmark.skipEmptyPages();

   _rowCount = retrieveRowCount();

   return true;
}

inline bool check(text_c ch1, text_c ch2, bool matchCase)
{
   if (matchCase) {
      return (ch1==ch2);
   }
   else return (_ELENA_::StrHelper::lower(ch1)==_ELENA_::StrHelper::lower(ch2));
}

bool Text :: compare(TextBookmark bookmark, text_t line, size_t len, bool matchCase, text_t terminators)
{
   if (terminators) {
      if (bookmark.go(-1)) {
         if((text_str(terminators).find((*bookmark._page).text[bookmark._offset])==-1)) {
            return false;
         }
         else bookmark.go(1);
      }
   }

   for (size_t i = 0 ; i < len ; i++) {
	  if (!check((*bookmark._page).text[bookmark._offset], line[i], matchCase))
	     return false;

	  if (!bookmark.go(1))
	     return (i==(len-1));
   }
   if (terminators) {
      return (text_str(terminators).find((*bookmark._page).text[bookmark._offset])!=-1);
   }
   else return true;
}

bool Text :: findWord(TextBookmark& bookmark, text_t line, bool matchCase, text_t terminators)
{
   validateBookmark(bookmark);
   bookmark.normalize();

   size_t len = getlength(line);
   text_c ch = line[0];
   while (true) {
      if (check((*bookmark._page).text[bookmark._offset], ch, matchCase)) {
         if (compare(bookmark, line, len, matchCase, terminators)) {
            bookmark._virtual_column = bookmark._column;
            return true;
         }
      }
      if (!bookmark.move(1))
         break;
   }
   return false;
}

void Text :: attachWatcher(_TextWatcher* watcher)
{
   _watchers.add(watcher);
}

void Text :: detachWatcher(_TextWatcher* watcher)
{
   _watchers.cut(watcher);
}

// --- TextHistory ---

struct HistoryWriter
{
   typedef TextHistory::Operation Operation;

   MemoryDump* _buffer;
   size_t      _lastLength, _lastPosition;
   Operation   _lastOperation;

   bool writeRecord(Operation operation, size_t& position, size_t& length, void* &line, size_t& offset)
   {
      MemoryWriter writer(_buffer);

      size_t freeSpace = _buffer->getFreeSpace();
      size_t shift = (operation == TextHistory::opInsert) ? _lastLength : 0;
      if (_lastOperation == operation && (_lastPosition + shift) == position) {
         if (freeSpace > length + getEndRecordSize()) {
            _lastLength += length;
            writer.write(line, length);
         }
         else {
            // if it is enough place to end buffer record
            size_t sublength = 0;
            if (freeSpace > getEndRecordSize()) {
               sublength = freeSpace - getEndRecordSize();
               writer.write(line, sublength);
            }

            writeEndRecord(writer, _lastLength + sublength);

            line = (char*)line + sublength;
            length -= sublength;
            if (operation == TextHistory::opInsert)
               position += sublength;

            return false;
         }
      }
      else {
         if (_lastOperation != TextHistory::opNone) {
            writeEndRecord(writer, _lastLength);
            freeSpace -= getEndRecordSize();
         }
         _lastOperation = operation;
         _lastPosition = position;
         if (freeSpace >= length + getStartRecordSize()) {
            _lastLength = length;
            writer.writeSize((operation == TextHistory::opInsert) ? position : position | ERASE_MODE);
            writer.write(line, length);
         }
         else {
            size_t sublength = 0;
            if (freeSpace > getStartRecordSize()) {
               sublength = freeSpace - getStartRecordSize();
               writer.writeSize((operation == TextHistory::opInsert) ? position : position | ERASE_MODE);
               writer.write(line, sublength);

               writeEndRecord(writer, sublength);
            }

            line = (char*)line + sublength;
            length -= sublength;
            if (operation == TextHistory::opInsert)
               position += sublength;

            return false;
         }
      }
      offset = writer.Position();
      return true;
   }

#ifdef _WIN32
   // starting record should include the position field (4), the operation field (4) and a at elast terminator symbol (2)
   size_t getStartRecordSize() { return 10; }

   // ending record should include the length field (4) and a terminator symbol (2)
   size_t getEndRecordSize() { return 6; }

   void writeEndRecord(MemoryWriter& writer, size_t length)
   {
      writer.writeChar((text_c)0);
      writer.writeSize(length);
   }

   bool write(Operation operation, size_t& position, size_t& length, void* &line, size_t& offset)
   {
      position = position << 1;
      length = length << 1;

      if (!writeRecord(operation, position, length, line, offset)) {
         position = position >> 1;
         length = length >> 1;

         return false;
      }
      else return true;
   }
#else
   // starting record should include the position field (4), the operation field (4) and a at elast terminator symbol (1)
   size_t getStartRecordSize() { return 9; }

   // ending record should include the length field (4) and a terminator symbol (1)
   size_t getEndRecordSize() { return 5; }

   void writeEndRecord(MemoryWriter& writer, int length)
   {
      writer.writeChar((text_c)0);
      writer.writeDWord(length);
   }

   bool write(Operation operation, size_t& position, size_t& length, void* &line, size_t& offset)
   {
      return writeRecord(operation, position, length, line, offset);
   }
#endif

   void end(size_t& offset)
   {
      MemoryWriter writer(_buffer);

      writeEndRecord(writer, _lastLength);

      offset = writer.Position();
   }

   HistoryWriter(MemoryDump* buffer, size_t lastLength, size_t lastPosition, Operation lastOperation)
   {
      _lastLength = lastLength;
      _lastPosition = lastPosition;
      _lastOperation = lastOperation;
      _buffer = buffer;
   }
};

class HistoryBackReader
{
   MemoryDump* _buffer;
   size_t      _offset;

public:
   size_t Position() const { return _offset; }

   size_t getSize()
   {
      _offset -= sizeof(size_t);

      size_t size = readData(_buffer, _offset);

      return size;
   }

#ifdef _WIN32
   size_t readLength()
   {
      // length saved in bytes should be converted to the wchar_t index
      return getSize() >> 1;
   }

   size_t readPosition(bool& eraseMode)
   {
      size_t value = getSize();

      size_t position = value & (ERASE_MODE - 1);
      eraseMode = testEraseMode(value);

      // position saved in bytes should be converted to wchar_t index
      return position >> 1;
   }

   const wchar_t* readLine(size_t length)
   {
      _offset -= ((length << 1) + 2);

      return (wchar_t*)_buffer->get(_offset);
   }
#else
   size_t readLength()
   {
      return getSize();
   }

   size_t readPosition(bool& eraseMode)
   {
      size_t value = getSize();

      size_t position = value & 0x7FFFFFFF;
      eraseMode = test(value, 0x80000000);

      return position;
   }

   const char* readLine(size_t length)
   {
      _offset -= (length + 1);

      return (char*)_buffer->get(_offset);
   }
#endif

   HistoryBackReader(MemoryDump* buffer, size_t offset)
   {
      _buffer = buffer;
      _offset = offset;
   }
};

class HistoryReader
{
   MemoryReader _reader;

public:
   size_t Position() { return _reader.Position(); }

#ifdef _WIN32
   int readLength()
   {
      // length saved in bytes should be converted to the wchar_t index
      return _reader.getDWord() >> 1;
   }

   size_t readPosition(bool& eraseMode)
   {
      size_t value;
      _reader.readSize(value);

      size_t position = value & (ERASE_MODE - 1);
      eraseMode = testEraseMode(value);

      // position saved in bytes should be converted to wchar_t index
      return position >> 1;
   }

   const wchar_t* readLine()
   {
      return _reader.getLiteral(DEFAULT_TEXT);
   }
#else
   int readLength()
   {
      return _reader.getDWord();
   }

   size_t readPosition(bool& eraseMode)
   {
      size_t value = _reader.getDWord();

      size_t position = value & 0x7FFFFFFF;
      eraseMode = test(value, 0x80000000);

      return position;
   }

   const char* readLine()
   {
      return _reader.getLiteral(DEFAULT_TEXT);
   }
#endif

   HistoryReader(MemoryDump* buffer, size_t offset)
      : _reader(buffer, offset)
   {
   }
};

TextHistory :: TextHistory(int capacity)
   : _buffer1(capacity), _buffer2(capacity)
{
   _locking = false;

   _lastOperation = opNone;
   _lastPosition = _lastLength = 0;

   _buffer = &_buffer1;
   _offset = 0;
   _previous = NULL;
}

void TextHistory :: onInsert(size_t position, size_t length, text_t line)
{
   if (_locking)
      return;

   trimData(_buffer, _offset);

   addRecord(opInsert, position, length, (void*)line);
}

void TextHistory :: onErase(size_t position, size_t length, text_t line)
{
   if (_locking)
      return;

   trimData(_buffer, _offset);

   addRecord(opErase, position, length, (void*)line);
}

void TextHistory :: addRecord(Operation operation, size_t position, size_t length, void* line)
{
   HistoryWriter writer(_buffer, _lastLength, _lastPosition, _lastOperation);

   if(!writer.write(operation, position, length, line, _offset)) {
      _lastOperation = opNone;
      _lastLength = writer._lastLength;
      _lastPosition = writer._lastPosition;

      switchBuffer();
      addRecord(operation, position, length, line);
   }
   else {
      _lastOperation = writer._lastOperation;
      _lastLength = writer._lastLength;
      _lastPosition = writer._lastPosition;
   }
}

bool TextHistory :: Bof() const
{
   return (_offset == 0 && (_previous == NULL));
}

bool TextHistory :: Eof() const
{
   if (_buffer->Length() ==_offset) {
      return true;
   }
   else return false;
}

void TextHistory :: switchBuffer()
{
   _previous = _buffer;
   if (&_buffer1==_buffer) {
      _buffer = &_buffer2;
   }
   else _buffer = &_buffer1;

   _buffer->clear();
   _offset = 0;
}

void TextHistory :: endRecord()
{
   HistoryWriter writer(_buffer, _lastLength, _lastPosition, _lastOperation);

   writer.end(_offset);

   _lastOperation = opNone;
}

void TextHistory :: undo(Text* text, TextBookmark& caret)
{
   if (Bof())
      return;

   text->validateBookmark(caret);

   if (_lastOperation != opNone) {
      endRecord();
   }

   if (_offset==0 && _previous) {
      _buffer = _previous;
      _offset = _previous->Length();
      _previous = NULL;
   }

   HistoryBackReader reader(_buffer, _offset);

   size_t length = reader.readLength();
   text_t line = reader.readLine(length);

   bool   eraseMode = false;
   size_t position = reader.readPosition(eraseMode);

   _locking = true;
   if (eraseMode) { // opDelete operation
      caret.moveOn(position - caret.getPosition());
      text->insertLine(caret, line, length);
      caret.moveOn(length);
   }
   else {                            // opInsert operation
      caret.moveOn(position - caret.getPosition());
      text->eraseLine(caret, length);
   }
   _offset = reader.Position();
   _locking = false;
}

void TextHistory :: redo(Text* text, TextBookmark& caret)
{
   if (Eof())
      return;

   text->validateBookmark(caret);

   HistoryReader reader(_buffer, _offset);

   bool   eraseMode = false;
   size_t position = reader.readPosition(eraseMode);

   text_t line = reader.readLine();
   size_t length = reader.readLength();

   _locking = true;
   if (eraseMode) { // opDelete operation
      caret.moveOn(position - caret.getPosition());
      text->eraseLine(caret, length);
   }
   else {                            // opInsert operation
      caret.moveOn(position - caret.getPosition());
      text->insertLine(caret, line, length);
   }
   _locking = false;
   _offset = reader.Position();

   if (_offset==_buffer->Length() && !_previous) {
      switchBuffer();
   }
}
