//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      DocumentView class header
//                                             (C)2021-2022, by Aleksey Rakov
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
   };

   struct Marker
   {
      pos_t style;

      bool operator ==(const Marker& m)
      {
         return style == m.style;
      }
   };
   typedef Map<int, Marker>   MarkerList;

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
      Text*                _text;
      MarkerList*          _markers;
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
      void format();

   public:
      void onUpdate(size_t position) override;
      void onInsert(size_t position, size_t length, text_t line) override;
      void onErase(size_t position, size_t length, text_t line) override;

      pos_t proceed(pos_t position, ReaderInfo& info);

      LexicalFormatter(Text* text, TextFormatterBase* formatter, MarkerList* markers);
      virtual ~LexicalFormatter();
   };

   // --- DocumentNotifier ---
   class DocumentNotifier
   {
   public:
      virtual void onDocumentUpdate() = 0;
   };

   typedef List<DocumentNotifier*> DocumentNotifiers;

   // --- Document ---
   class DocumentView
   {
   public:
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

            this->bm.invalidate();
         }
      };

      struct Status
      {
         bool caretChanged;
         bool maxColChanged;
         bool frameChanged;
         bool selelectionChanged;
         bool formatterChanged;

         bool isViewChanged(bool reset = true)
         {
            bool flag = formatterChanged | frameChanged | selelectionChanged;

            if (reset)
               formatterChanged = frameChanged = selelectionChanged = false;

            return flag;
         }

         void reset()
         {
            caretChanged = false;
            maxColChanged = false;
            frameChanged = false;
            selelectionChanged = false;
            formatterChanged = false;
         }

         Status()
         {
            reset();
         }
      };

      friend struct LexicalReader;

   protected:
      Text*             _text;
      LexicalFormatter  _formatter;

      Point             _size;
      TextBookmark      _frame;
      TextBookmark      _caret;
      pos_t             _selection;

      int               _maxColumn;

      MarkerList        _markers;
      DocumentNotifiers _notifiers;

      pos_t format(LexicalReader& reader);
      
   public:
      Status status;

      void attachNotifier(DocumentNotifier* notifier)
      {
         _notifiers.add(notifier);
      }

      void removeNotifier(DocumentNotifier* notifier)
      {
         _notifiers.cut(notifier);
      }

      void addMarker(int row, pos_t style)
      {
         _markers.add(row, { style });

         status.formatterChanged = true;
      }
      void removeMarker(int row, pos_t style)
      {
         _markers.erase(row, { style });

         status.formatterChanged = true;
      }

      virtual void resize(Point size);

      TextBookmark getCaretBookmark() { return _caret; }

      Point getFrame() const { return _frame.getCaret(); }
      Point getCaret(bool virtualOne = true) const { return _caret.getCaret(virtualOne); }
      Point getSize() const { return _size; }

      int getRowCount() const { return _text->getRowCount(); }
      int getMaxColumn() const { return _maxColumn; }

      void setCaret(int column, int row, bool selecting);
      void setCaret(Point caret, bool selecting)
      {
         setCaret(caret.x, caret.y, selecting);
      }

      void vscroll(int offset);
      void hscroll(int offset);

      void moveRight(bool selecting);
      void moveLeft(bool selecting);
      void moveUp(bool selecting);
      void moveDown(bool selecting);

      void moveRightToken(bool selecting, bool trimWhitespace = false);
      void moveLeftToken(bool selecting);
      void moveFrameUp();
      void moveFrameDown();

      void moveToFrame(int column, int row, bool selecting);

      void notifyOnChange();

      DocumentView(Text* text, TextFormatterBase* formatter);
      virtual ~DocumentView() = default;
   };
}

#endif
