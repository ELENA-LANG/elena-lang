//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Text class implementation
//                                              (C)2005-2013, by Alexei Rakov
//---------------------------------------------------------------------------

#include "text.h"

using namespace _GUI_;
using namespace _ELENA_;

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

bool TextBookmark :: go(int disp, bool allowEmpty)
{
   if (disp < 0) {
      while (_offset < -disp) {
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

bool TextBookmark :: move(int disp)
{
   if (disp < 0) {
      // go to the next page start if <end of the page>
      if(_status == BM_EOP)
         goToNextPage();

      while (disp < 0) {
         disp++;
         if (!prevChar(disp))
            return false;

         if ((*_page).text[_offset]==_LF) {
            _length = BM_INVALID;
            _row--;

            TextBookmark bm = *this;
            bm.moveToPrevBOL();

            _column = _length = bm.getLength();

            go(-1);
            disp++;

            //if (valid && (*_page).text[_offset]==_CF) {
            //   go(-1, valid);
            //   disp++;
            //}

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

         if ((*_page).text[_offset]==_CF) {
            _row++;
            _column = 0;
            _length = BM_INVALID;

            if(!go(1))
               return false;

            if ((*_page).text[_offset]==_LF) {
               valid = go(1);

               disp--;
            }
         }
         else if ((*_page).text[_offset]==_LF) {
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

void TextBookmark :: moveToClosestRow(size_t row)
{
   moveToStart();
   if (row != _row) {
      size_t closestRow = 0;
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

void TextBookmark :: moveToClosestColumn(size_t column)
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

   if ((*_page).text[_offset]==_LF) {
      _row--;
      if (!go(-1))
         return false;
   }
   // move until BOL
   while ((*_page).text[_offset]!=_LF) {
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
   while ((*_page).text[_offset]!=_LF) {
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

   if (_CF != 0) {
      text_c ch = (*bm._page).text[bm._offset];
      while (ch != _CF && ch != _LF) {
         if (!bm.move(1))
            break;

         ch = (*bm._page).text[bm._offset];
      }
   }
   else {
      bool valid = true;
      while (valid && (*bm._page).text[bm._offset]!=_LF) {
         valid = bm.move(1);
      }
      if (valid)
         bm.move(-1);
   }
   return bm._column - _column;
}

bool TextBookmark :: moveTo(size_t column, size_t row)
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
   if (column > getLength())
      column = getLength();

   moveToClosestColumn(column);

   return true;
}

bool TextBookmark :: moveOn(int disp)
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

Text :: Text()
{
   _LF = 0x0A;
   _CF = 0x0D;

   _rowCount = 0;
}

Text :: ~Text()
{
}

void Text :: validateBookmark(TextBookmark& bookmark)
{
   if (!bookmark.isValid()) {
      bookmark._LF = _LF;
      bookmark._CF = _CF;
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
   int used = (*page).used;
   text_c* text = (*page).text;

   (*page).rows = 0;
   for (int i = 0 ; i < used ; i++) {
      if (text[i]==_LF) {
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

size_t Text :: getRowLength(size_t row)
{
   if (row < _rowCount) {
      TextBookmark bookmark;
      validateBookmark(bookmark);

      bookmark.moveTo(0, row);

      return bookmark.getLength();
   }
   else return 0;
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
      for (size_t i = 0 ; i < count ; i++) {
         if (stopOnEOL && (line[i]==_CF || line[i]==_LF)) {
            bookmark.moveOn(i);
            return;
         }
         else if (line[i]=='\t') {
            int disp = _ELENA_::calcTabShift(col, Text::TabSize);
            writer.fillText(_T(" "), 1, disp + diff);
            diff = 0;
            col += disp;
         }
         else {
            writer.write(&line[i], 1);
            col++/* += TextBookmark::charLength(line, offset)*/;
         }
      }
      if (!bookmark.moveOn(count))
         break;

      length -= count;
   }
}

void Text :: copyTo(TextBookmark bookmark, text_c* buffer, int length)
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
      _ELENA_::StringHelper::copy(buffer, (*bookmark._page).text + bookmark._offset, copied, copied);

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
   ref_t position = bookmark.getPosition();

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
            _ELENA_::StringHelper::copy(newPage.text, (*page).text + offset, size, size);

            _pages.insertAfter(page, newPage);

            if (!checkRowCount) {
               refreshPage(page);
            }
            refreshNextPage(page);

            if (size > length)
               size = length;
         }
         else _ELENA_::StringHelper::move((*page).text + offset + size, (*page).text + offset, (*page).used - offset);
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
      _ELENA_::StringHelper::copy((*page).text + offset, s, size, size);

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
            _ELENA_::StringHelper::copy((*page).text + offset, (*page).text + offset + size, l, l);
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
   if (_CF != 0) {
      ch[0] = _CF;
      ch[1] = _LF;
      insert(bookmark, ch, 2, true);
   }
   else {
      ch[0] = _LF;
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
         erase(bookmark, 2, true);

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
   else return (_ELENA_::StringHelper::lower(ch1)==_ELENA_::StringHelper::lower(ch2));
}

bool Text :: compare(TextBookmark bookmark, text_t line, int len, bool matchCase, text_t terminators)
{
   if (terminators) {
      if (bookmark.go(-1)) {
         if((_ELENA_::StringHelper::find(terminators, (*bookmark._page).text[bookmark._offset])==-1)) {
            return false;
         }
         else bookmark.go(1);
      }
   }

   for (int i = 0 ; i < len ; i++) {
	  if (!check((*bookmark._page).text[bookmark._offset], line[i], matchCase))
	     return false;

	  if (!bookmark.go(1))
	     return (i==(len-1));
   }
   if (terminators) {
      return (_ELENA_::StringHelper::find(terminators, (*bookmark._page).text[bookmark._offset])!=-1);
   }
   else return true;
}

bool Text :: findWord(TextBookmark& bookmark, text_t line, bool matchCase, text_t terminators)
{
   validateBookmark(bookmark);
   bookmark.normalize();

   int len = getlength(line);
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
            writer.writeDWord((operation == TextHistory::opInsert) ? position : position | 0x80000000);
            writer.write(line, length);
         }
         else {
            size_t sublength = 0;
            if (freeSpace > getStartRecordSize()) {
               sublength = freeSpace - getStartRecordSize();
               writer.writeDWord((operation == TextHistory::opInsert) ? position : position | 0x80000000);
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

   void writeEndRecord(MemoryWriter& writer, int length)
   {
      writer.writeChar((text_c)0);
      writer.writeDWord(length);
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

   int getDWord()
   {
      _offset -= 4;

      int dword;
      _buffer->read(_offset, &dword, 4);

      return dword;
   }

#ifdef _WIN32
   int readLength()
   {
      // length saved in bytes should be converted to the wchar_t index
      return getDWord() >> 1;
   }

   size_t readPosition(bool& eraseMode)
   {
      size_t value = getDWord();

      size_t position = value & 0x7FFFFFFF;
      eraseMode = test(value, 0x80000000);

      // position saved in bytes should be converted to wchar_t index
      return position >> 1;
   }

   const wchar_t* readLine(size_t length)
   {
      _offset -= ((length << 1) + 2);

      return (wchar_t*)_buffer->get(_offset);
   }
#else
   int readLength()
   {
      return getDWord();
   }

   size_t readPosition(bool& eraseMode)
   {
      size_t value = getDWord();

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
      size_t value = _reader.getDWord();

      size_t position = value & 0x7FFFFFFF;
      eraseMode = test(value, 0x80000000);

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

   _buffer->trim(_offset);

   addRecord(opInsert, position, length, (void*)line);
}

void TextHistory :: onErase(size_t position, size_t length, text_t line)
{
   if (_locking)
      return;

   _buffer->trim(_offset);

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
