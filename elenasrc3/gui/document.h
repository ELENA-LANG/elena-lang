//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      DocumentView class header
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef DOCUMENT_H
#define DOCUMENT_H

#include "text.h"

namespace elena_lang
{
   constexpr auto INDEX_STEP = 0x100;
   constexpr auto INDEX_ORDER = 8;

   // --- FormatterInfo ---
   struct FormatterInfo
   {
      pos_t  style;
      bool   lookAhead;
      text_c state;
   };

   // --- ReaderInfo ---
   struct ReaderInfo
   {
      pos_t style;
      pos_t step;

      int   row;

      bool  newLine;
      bool  bandStyle;
      bool  toggleMark;
   };

   struct Marker
   {
      pos_t style;
      bool  toggleMark;

      bool operator ==(const Marker& m)
      {
         return style == m.style;
      }
      bool operator !=(const Marker& m)
      {
         return style != m.style;
      }
   };
   typedef CachedMemoryMap<int, Marker, 5>   MarkerList;
   typedef CachedMemoryMap<pos_t, pos_t, 2>  HighlightList;

   // --- TextFormatterBase ---
   class TextFormatterBase
   {
   public:
      virtual void start(FormatterInfo& info) = 0;

      virtual bool next(text_c ch, FormatterInfo& info, pos_t& lastStyle) = 0;
   };

   // --- LexicalFormatter ---
   class LexicalFormatter : public TextWatcherBase
   {
      bool                 _enabled;

      Text*                _text;
      MarkerList*          _markers;
      HighlightList*       _highlights;
      MemoryDump           _indexes;
      MemoryDump           _lexical;

      TextFormatterBase*   _formatter;

      pos_t retrievePosition(pos_t position)
      {
         position >>= INDEX_ORDER;

         MemoryReader reader(&_indexes, position * sizeof(pos_t));

         return reader.getPos();
      }

      bool checkMarker(ReaderInfo& info);
      bool checkPrecedingHighlight(pos_t start, pos_t& end);
      void format();

   public:
      void setEnabled(bool enabled);

      void onUpdate(size_t position) override;
      void onInsert(size_t position, size_t length, const_text_t line) override;
      void onErase(size_t position, size_t length, const_text_t line) override;

      pos_t proceed(pos_t position, ReaderInfo& info);

      LexicalFormatter(Text* text, TextFormatterBase* formatter, MarkerList* markers, HighlightList* highlights);
      virtual ~LexicalFormatter();
   };

   // --- DocumentChangeStatus ---
   struct DocumentChangeStatus
   {
      bool caretChanged;
      bool maxColChanged;
      bool frameChanged;
      bool selelectionChanged;
      bool formatterChanged;
      bool textChanged;
      bool modifiedChanged;
      bool readOnlyChanged;
      bool modeChanged;

      bool isViewChanged()
      {
         bool flag = formatterChanged || frameChanged || textChanged || selelectionChanged;

         return flag;
      }

      void reset()
      {
         caretChanged = false;
         maxColChanged = false;
         frameChanged = false;
         modifiedChanged = false;
         selelectionChanged = false;
         formatterChanged = false;
         textChanged = false;
         readOnlyChanged = false;
         modeChanged = false;
      }

      DocumentChangeStatus()
      {
         reset();
      }
      DocumentChangeStatus(bool dirty)
      {
         reset();
         if (dirty) {
            caretChanged = true;
            maxColChanged = true;
            frameChanged = true;
            selelectionChanged = true;
            textChanged = true;
            modeChanged = true;
         }
      }
   };

   // --- DocumentNotifier ---
   class DocumentNotifier
   {
   public:
      virtual void onDocumentUpdate(DocumentChangeStatus& changeStatus) = 0;
   };

   typedef List<DocumentNotifier*> DocumentNotifiers;

   // --- Document ---
   class DocumentView : public TextWatcherBase
   {
   public:
      static int VerticalScrollOffset;

      struct LexicalReader : ReaderInfo
      {
         Rectangle     region;

         DocumentView* docView;
         TextBookmark  bm;

         void seekCurrentLine();
         bool readCurrentLine(TextWriter<text_c>& writer, pos_t length);

         void readFirst(TextWriter<text_c>& writer, pos_t length);
         bool readNext(TextWriter<text_c>& writer, pos_t length);

         LexicalReader(DocumentView* docView)
         {
            this->docView = docView;
            this->step = this->style = 0;
            this->row = 0;
            this->newLine = false;
            this->bandStyle = false;
            this->toggleMark = false;

            this->bm.invalidate();
         }
      };

      struct Status
      {
         bool readOnly;
         bool modifiedMode;
         bool unnamed;
         bool included;
         bool overwriteMode;

         int  rowDifference;

         bool oldModified;
         bool oldSelected;

         void reset()
         {
            modifiedMode = false;
            readOnly = false;
            unnamed = included = false;
            overwriteMode = false;
            oldSelected = oldModified = false;

            rowDifference = 0;
         }

         Status()
         {
            reset();
         }
      };

      enum class IndentDirection
      {
         None = 0,
         Left,
         Right
      };

      friend struct LexicalReader;

   protected:
      Status            status;

      Text*             _text;
      TextHistory       _undoBuffer;
      LexicalFormatter  _formatter;
      HighlightList     _highlights;

      Point             _size;
      TextBookmark      _frame;
      TextBookmark      _caret;
      int               _selection;

      int               _maxColumn;

      bool              _autoIndent;

      MarkerList        _markers;

      pos_t format(LexicalReader& reader);

      void onInsert(size_t position, size_t length, const_text_t line) override;
      void onUpdate(size_t position) override;
      void onErase(size_t position, size_t length, const_text_t line) override;

      TextBookmark getCaretBookmark() { return _caret; }

      void setCaret(int column, int row, bool selecting, DocumentChangeStatus& changeStatus);

      text_t getCurrentLine(disp_t disp, size_t& length);      

      IndentDirection IsAutoIndent(text_c ch);
      disp_t calcAutoIndent(text_c currentChar);

   public:
      void addMarker(int row, pos_t style, bool instanteMode, bool togleMark, DocumentChangeStatus& changeStatus)
      {
         _markers.add(row, { style, togleMark });

         changeStatus.formatterChanged = true;
      }
      void removeMarker(int row, pos_t style, DocumentChangeStatus& changeStatus)
      {
         _markers.erase(row, { style });

         changeStatus.formatterChanged = true;
      }
      bool removeMarker(pos_t style, DocumentChangeStatus& changeStatus)
      {
         if (_markers.count() > 0) {
            int row = _markers.retrieve<pos_t>(-1, style, [](pos_t arg, int key, Marker item)
            {
               return item.style == arg;
            });

            if (row != -1) {
               _markers.erase(row, { style });

               changeStatus.formatterChanged = true;
               return true;
            }
         }

         return false;
      }

      bool clearHighlights()
      {
         if (_highlights.count() > 0) {
            _highlights.clear();

            return true;
         }
         return false;
      }

      void addHighlight(pos_t position, pos_t style)
      {
         _highlights.add(position, style);
      }

      Text* getText() { return _text; }

      TextBookmark getCurrentTextBookmark() { return _caret; }

      Point getFrame() const { return _frame.getCaret(); }
      Point getCaret(bool virtualOne = true) const { return _caret.getCaret(virtualOne); }
      void setCaret(Point caret, bool selecting, DocumentChangeStatus& changeStatus)
      {
         setCaret(caret.x, caret.y, selecting, changeStatus);
      }
      void setReadOnlyMode(bool mode)
      {
         status.readOnly = mode;
      }
      void setHighlightMode(bool enabled)
      {
         _formatter.setEnabled(enabled);
      }

      void setOverwriteMode(DocumentChangeStatus& changeStatus, bool mode);

      int getRowCount() const { return _text->getRowCount(); }
      int getMaxColumn() const { return _maxColumn; }
      disp_t getSelectionLength();
      disp_t getCurrentLineLength();

      bool hasSelection() const { return (_selection != 0); }
      bool isReadOnly() { return status.readOnly; }
      bool isUnnamed() { return status.unnamed; }
      bool isModified() { return status.modifiedMode; }
      bool isIncluded() { return status.included; }
      bool isOverwriteMode() { return status.overwriteMode; }

      void markAsUnnamed()
      {
         status.unnamed = true;
      }
      void markAsModified()
      {
         status.modifiedMode = true;
      }
      void markAsInclued()
      {
         status.included = true;
      }
      void markAsExcluded()
      {
         status.included = false;
      }

      text_c getCurrentChar();

      Point getSize() const { return _size; }

      virtual void setSize(Point size);

      virtual bool canUndo();
      virtual bool canRedo();

      void vscroll(DocumentChangeStatus& changeStatus, int offset);
      void hscroll(DocumentChangeStatus& changeStatus, int offset);

      void moveHome(DocumentChangeStatus& changeStatus, bool selecting);
      void moveFirst(DocumentChangeStatus& changeStatus, bool selecting);

      void moveEnd(DocumentChangeStatus& changeStatus, bool selecting);
      void moveLast(DocumentChangeStatus& changeStatus, bool selecting);

      void moveFrameDown(DocumentChangeStatus& changeStatus, int frameOffset);
      void moveDown(DocumentChangeStatus& changeStatus, bool selecting);

      void moveFrameUp(DocumentChangeStatus& changeStatus, int frameOffset);
      void moveUp(DocumentChangeStatus& changeStatus, bool selecting);

      void moveLeftToken(DocumentChangeStatus& changeStatus, bool selecting);
      void moveLeft(DocumentChangeStatus& changeStatus, bool selecting);

      void moveRightToken(DocumentChangeStatus& changeStatus, bool selecting, bool trimWhitespace = false);
      void moveRight(DocumentChangeStatus& changeStatus, bool selecting);

      void movePageUp(DocumentChangeStatus& changeStatus, bool selecting);
      void movePageDown(DocumentChangeStatus& changeStatus, bool selecting);

      void moveToFrame(DocumentChangeStatus& changeStatus, int column, int row, bool selecting);

      void copySelection(text_c* text);
      void copyCurrentLine(text_c* text);
      void copyText(text_c* text, disp_t length);

      void insertChar(DocumentChangeStatus& changeStatus, text_c ch)
      {
         insertChar(changeStatus, ch, 1);
      }
      void insertChar(DocumentChangeStatus& changeStatus, text_c ch, size_t number, bool advancing = true);
      void insertNewLine(DocumentChangeStatus& changeStatus);
      void insertLine(DocumentChangeStatus& changeStatus, const_text_t text, size_t length);

      virtual void blockInserting(DocumentChangeStatus& changeStatus, const_text_t subs, size_t length);
      virtual void blockDeleting(DocumentChangeStatus& changeStatus, const_text_t subs, size_t length);

      bool eraseSelection(DocumentChangeStatus& changeStatus);
      void eraseChar(DocumentChangeStatus& changeStatus, bool moveback);

      void trim(DocumentChangeStatus& changeStatus);
      void eraseLine(DocumentChangeStatus& changeStatus);
      void duplicateLine(DocumentChangeStatus& changeStatus);

      void toLowercase(DocumentChangeStatus& changeStatus);
      void toUppercase(DocumentChangeStatus& changeStatus);

      void undo(DocumentChangeStatus& changeStatus);
      void redo(DocumentChangeStatus& changeStatus);

      virtual void tabbing(DocumentChangeStatus& changeStatus, text_c space, size_t count, bool indent);

      void save(path_t path);

      void refresh(DocumentChangeStatus& changeStatus);

      bool findLine(DocumentChangeStatus& changeStatus, const_text_t text, bool matchCase, bool wholeWord);   

      DocumentView(Text* text, TextFormatterBase* formatter, bool autoIndent);
      virtual ~DocumentView();
   };
}

#endif
