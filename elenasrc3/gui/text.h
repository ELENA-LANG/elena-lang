//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Text class header
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef TEXT_H
#define TEXT_H

namespace elena_lang
{
   constexpr pos_t PAGE_SIZE = 0x100;

   constexpr pos_t BM_INVALID = -1;
   constexpr pos_t BM_VALID   = 0;
   constexpr pos_t BM_EOP     = 1;    // end of page
   constexpr pos_t BM_EOT     = 2;    // end of text

   // --- TextWatcherBase ---
   class TextWatcherBase
   {
   public:
      virtual void onUpdate(size_t position) = 0;
      virtual void onInsert(size_t position, size_t length, text_t line) = 0;
      virtual void onErase(size_t position, size_t length, text_t line) = 0;

      virtual ~TextWatcherBase() = default;
   };

   typedef List<TextWatcherBase*>  TextWatchers;

   // --- Page ---
   enum class EOLMode
   {
      None,
      LF,
      CRLF
   };

   struct Page
   {
      size_t   used;
      int      rows;
      text_c   text[PAGE_SIZE];

      Page()
      {
         used = 0;
         rows = 0;
      }
      Page(size_t size)
      {
         rows = 0;
         used = size;
      }
      Page(const Page& page)
      {
         rows = page.rows;
         used = page.used;
         StrConvertor::copy(text, page.text, used, used);
      }
   };

   typedef BList<Page>  Pages;

   // forward declaration
   class Text;
   class TextBookmarkReader;

   // --- TextBookmark ---
   struct TextBookmark
   {
      friend class Text;
      friend class TextBookmarkReader;

   private:
      EOLMode         _mode;

      // bookmark text position
      int             _column;
      int             _virtual_column;
      int             _row;

      // bookmark stream position
      pos_t           _status;
      size_t          _pagePosition;
      size_t          _offset;
      size_t          _length;
      Pages::Iterator _page;

      void set(Pages* pages);

      void normalize();                                 // make sure _offset is within page text range

      bool goToPreviousPage(bool allowEmpty = false);   // do not trace caret coordinate
      bool goToNextPage(bool allowEmpty = false);

      bool go(disp_t disp, bool allowEmpty = false);    // moving without tracing caret coordinate

      void refreshStatus()
      {
         if ((*_page).used == _offset) {
            _status = _page.last() ? BM_EOT : BM_EOP;
         }
         else _status = BM_VALID;
      }

      bool prevChar(disp_t& disp)
      {
#ifdef __GNUG__
         if (go(-1)) {
            if (_status == 0 && test((*_page).text[_offset], 0x80)) {
               do {
                  if (!go(-1))
                     return false;
               } while (!test((*_page).text[_offset], 0xC0));

               return true;
            }
         }
         else return false;
#elif _MSC_VER
         if (go(-1)) {
            if ((unsigned)(*_page).text[_offset] >= UNI_SUR_LOW_START) {
               disp++;
               return go(-1);
            }
            else return true;
         }
         else return false;
#endif
      }

      bool nextChar(disp_t& disp)
      {
#ifdef __GNUG__
         if (test((*_page).text[_offset], 0x80)) {
            if ((unsigned char)(*_page).text[_offset] >= 0xF0) {
               go(3);
               disp -= 3;
            }
            else if ((unsigned char)(*_page).text[_offset] >= 0xE0) {
               go(2);
               disp -= 2;
            }
            else {
               go(1);
               disp--;
            }
         }
         return go(1);
#elif _MSC_VER
         if ((unsigned)(*_page).text[_offset] >= UNI_SUR_HIGH_START) {
            disp--;
            return go(2);
         }
         else return go(1);
#endif
      }

   public:
      static size_t charLength(const text_c* s, size_t offset)
      {
#ifdef __GNUG__
         if (test(s[offset], 0x80)) {
            if ((unsigned char)s[offset] >= 0xF0) {
               return 4;
            }
            else if ((unsigned char)s[offset] >= 0xE0) {
               return 3;
            }
            else return 2;
         }
         return 1;
#else
         if (s[offset] >= UNI_SUR_HIGH_START) {
            return 2;
         }
         else return 1;
#endif
      }

      TextBookmark& operator =(const TextBookmark& bookmark)
      {
         this->_status = bookmark._status;
         this->_column = bookmark._column;
         this->_virtual_column = bookmark._virtual_column;
         this->_row = bookmark._row;
         this->_pagePosition = bookmark._pagePosition;
         this->_page = bookmark._page;
         this->_offset = bookmark._offset;

         this->_length = bookmark._length;
         this->_mode = bookmark._mode;

         return *this;
      }

      static bool isNewLineCh(const text_c ch)
      {
         return ch == 10 || ch == 13;
      }

      bool isValid() const { return _status != BM_INVALID; }

      Point getCaret(bool isVirtual = true) const
      {
         return Point(isVirtual ? _virtual_column : _column, _row);
      }
      int row() const { return _row; }
      int column(bool virtualOne = true) const { return virtualOne ? _virtual_column : _column; }

      pos_t position() const { return static_cast<pos_t>(_pagePosition + _offset); }
      size_t longPosition() const { return _pagePosition + _offset; }

      size_t length();
      pos_t length_pos()
      {
         return (pos_t)length();
      }
      int length_int()
      {
         return (int)length();
      }

      int getVirtualDiff() const { return _column - _virtual_column; }

      bool move(disp_t disp);
      bool moveToClosestPosition(size_t position);
      void moveToStart();
      void moveToClosestRow(int row);
      void moveToClosestColumn(int column);

      bool moveToPrevBOL();
      bool moveToNextBOL();
      bool moveTo(int column, int row);
      bool moveOn(disp_t disp);

      size_t seekEOL();

      void invalidate()
      {
         _status = BM_INVALID;
         _length = NOTFOUND_POS;
      }

      bool isEOF() const { return _page.last() && _offset >= (int)(*_page).used; }
      bool isEOL() { return length_pos() <= (pos_t)_column; }

      TextBookmark();
      ~TextBookmark();
   };

   // --- DocReader ---
   class TextBookmarkReader
   {
      Text* _text;
      TextBookmark _bookmark;

   public:
      text_t readLine(pos_t& length);

      pos_t position() const
      {
         return _bookmark.position();
      }

      bool goTo(disp_t disp)
      {
         return _bookmark.go(disp);
      }

      TextBookmarkReader(Text* text);
   };

   // --- Text ---
   class Text
   {
      EOLMode      _mode;
      FileEncoding _encoding;

      Pages        _pages;
      int          _rowCount;

      TextWatchers _watchers;

      void refreshPage(Pages::Iterator& page);
      void refreshNextPage(Pages::Iterator page);

      int retrieveRowCount();

      void insert(TextBookmark bookmark, text_t s, size_t length, bool checkRowCount);
      void erase(TextBookmark bookmark, size_t length, bool checkRowCount);

   public:
      static int TabSize;

      int getRowCount() const { return _rowCount; }

      void validateBookmark(TextBookmark& bookmark);

      void copyLineTo(TextBookmark& bookmark, TextWriter<text_c>& writer, pos_t length, bool stopOnEOL);
      void copyLineToX(TextBookmark& bookmark, TextWriter<text_c>& writer, pos_t length, int x);

      text_t getLine(TextBookmark& bookmark, pos_t& length);

      void create();
      bool load(path_t path, FileEncoding encoding, bool autoDetecting);
      void save(path_t path);

      bool insertChar(TextBookmark& bookmark, text_c ch);
      bool insertLine(TextBookmark& bookmark, text_t s, size_t length);
      bool insertNewLine(TextBookmark& bookmark);

      bool eraseChar(TextBookmark& bookmark);
      bool eraseLine(TextBookmark& bookmark, size_t length);

      void attachWatcher(TextWatcherBase* watcher);
      void detachWatcher(TextWatcherBase* watcher);

      Text(EOLMode mode);
      virtual ~Text();
   };

   // --- TextHistory ---
   class TextHistory : public TextWatcherBase
   {
   public:
      typedef MemoryDump   Buffer;
      typedef MemoryWriter BufferWriter;

      enum class Operation { None, Insert, Erase };

      class HistoryWriter
      {
         void writeEndRecord(MemoryWriter& writer, pos_t length);

         bool writeRecord(Buffer* buffer, Operation operation, pos_t& position, pos_t& length,
            void* &line, pos_t& offset);

      public:
         pos_t     lastLength, lastPosition;
         Operation lastOperation;

         bool write(Buffer* buffer, Operation operation, pos_t& position, pos_t& length,
            void*& line, pos_t& offset);

         void endRecord(Buffer* buffer, pos_t& offset);

         HistoryWriter();
      };

      class HistoryBackReader
      {
         Buffer* _buffer;
         pos_t   _offset;

      public:
         pos_t getLength();
         void* getLine(pos_t length);
         pos_t getPosition(bool& eraseMode);

         HistoryBackReader(Buffer* buffer, pos_t offset);
      };

   private:
      Buffer        _buffer1;
      Buffer        _buffer2;

      bool          _locking;

      pos_t         _offset;
      Buffer*       _buffer;
      Buffer*       _previous;

      HistoryWriter _writer;

      void switchBuffer();

      void writeOperation(Operation operation, pos_t position,
         pos_t length, void* line);

   public:
      bool eof() const;
      bool bof() const;

      void onUpdate(size_t position) override;
      void onInsert(size_t position, size_t length, text_t line) override;
      void onErase(size_t position, size_t length, text_t line) override;

      void undo(Text* text, TextBookmark& caret);
      void redo(Text* text, TextBookmark& caret);

      TextHistory(pos_t capacity);
      virtual ~TextHistory() = default;
   };
}

#endif
