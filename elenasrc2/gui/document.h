//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Document class header
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef documentH
#define documentH

#include "guicommon.h"
#include "text.h"

namespace _GUI_
{

// --- built-in styles ---
#define STYLE_DEFAULT                           0
#define STYLE_MARGIN                            1
#define STYLE_SELECTION                         2

// --- LexicalInfo ---

struct LexicalInfo
{
   _ELENA_::pos_t step;
   _ELENA_::pos_t style;

   int            row;
   TextBookmark   bm;

   bool           bandStyle;
   bool           newLine;
   bool           marker;
};

// --- LexicalStyler ---

#define INDEX_STEP  0x100
#define INDEX_ORDER 8

class LexicalStyler : public _TextWatcher
{
   bool  _enabled;

   Text* _text;

   _ELENA_::MemoryDump _index;
   _ELENA_::MemoryDump _lexic;

   // Lexical parser states
   int     _defaultStyle;
   text_c  _lookaheadState;
   text_c  _startState;

   // Lexical parser functions
   text_c(*_makeStep)   (text_c ch, text_c state);
   _ELENA_::pos_t(*_defineStyle)(text_c state, _ELENA_::pos_t style);

   _ELENA_::pos_t retrievePosition(_ELENA_::pos_t position)
   {
      position = (position >> INDEX_ORDER);

      _ELENA_::MemoryReader reader(&_index, position * 4);

      return reader.getDWord();
   }

   virtual void onUpdate(size_t position)
   {
      parse();
   }

   virtual void onInsert(size_t, size_t, text_t )
   {
   }

   virtual void onErase(size_t, size_t, text_t)
   {
   }

public:
   void setEnabled(bool enabled);

   void parse();

   virtual _ELENA_::pos_t proceed(_ELENA_::pos_t position, LexicalInfo& info);

   virtual bool addMarker(HighlightInfo info, _ELENA_::pos_t bandStyle, _ELENA_::pos_t style)
   {
      return false;
   }
   virtual void removeMarker(int row, _ELENA_::pos_t bandStyle)
   {
   }

   LexicalStyler(Text* text, _ELENA_::pos_t defaultStyle, text_c lookaheadState, text_c startState,
      text_c(*makeStep)(text_c ch, text_c state), _ELENA_::pos_t(*defineStyle)(text_c state, _ELENA_::pos_t style));
};

// --- Document ---

#define UNDO_BUFFER_SIZE 0x40000

// --- DocStatus ---

struct DocStatus
{
   // mode
   bool readOnly;

   // text status
   bool maxColChanged;

   // view status
   bool frameChanged;
   bool selelectionChanged;
   bool caretChanged;
   bool modifiedMode;
   bool unnamed;
   bool included;
   bool overwriteMode;

   bool oldModified;
   bool oldOvewrite;

   int  rowDifference;

   bool isModeChanged()
   {
      bool changed = (modifiedMode != oldModified) || (overwriteMode != oldOvewrite);

      oldModified = modifiedMode;
      oldOvewrite = overwriteMode;

      return changed;
   }

   bool isViewChanged(bool reset = true)
   {
      bool flag = frameChanged | selelectionChanged;

      if (reset)
         frameChanged = selelectionChanged = false;

      return flag;
   }

   void init()
   {
      readOnly = false;

      maxColChanged = false;
      frameChanged = false;
      selelectionChanged = false;
      oldModified = modifiedMode = false;
      unnamed = false;
      included = false;
      overwriteMode = false;
      oldOvewrite = true;     // to trigger mode change

      caretChanged = false;

      rowDifference = 0;
   }
};

class Document  : public _TextWatcher
{
public:
   struct Reader : public LexicalInfo
   {
      Rectangle    _region;
      Document*    _doc;

      void initCurrentLine();
      bool readCurrentLine(_ELENA_::TextWriter& writer, size_t length);

      bool readFirst(_ELENA_::TextWriter& writer, size_t length);
      bool readNext(_ELENA_::TextWriter& writer, size_t length);

      Reader(Document* doc)
      {
         style = 0;
         step = 0;
         marker = newLine = bandStyle = false;
         row = 0;
         bm.invalidate();

         _doc = doc;
      }
   };

protected:
   friend struct Reader;

   int            _encoding;
   Text*          _text;
   TextHistory    _undoBuffer;
   LexicalStyler* _styler;

   Point         _size;
   TextBookmark  _frame;
   TextBookmark  _caret;
   disp_t        _selection;

   int           _maxColumn;

   virtual void onUpdate(size_t position);
   virtual void onInsert(size_t position, size_t length, text_t line);
   virtual void onErase(size_t position, size_t length, text_t line);

   size_t defineStyle(Reader& reader);

   bool eraseSelection();

   int retrieveColumn(int row, int disp);

public:
   DocStatus status;

   bool hasSelection() const { return (_selection != 0); }
   disp_t getSelectionLength();

   Text* getText() const { return _text; }
   TextBookmark getCurrentTextBookmark() { return _caret; }

   Point getFrame() const { return _frame.getCaret(); }
   Point getCaret(bool _virtual = true) const { return _caret.getCaret(_virtual); }
   Point getSize() const { return _size; }

   int getRowCount() const { return _text->getRowCount(); }
   int getMaxColumn() const { return _maxColumn; }

   bool findLine(text_t text, bool matchCase, bool wholeWord);

   virtual bool canUndo();
   virtual bool canRedo();

   void setCaret(int column, int row, bool selecting);
   void setCaret(HighlightInfo info, bool selecting);
   void setCaret(Point caret, bool selecting)
   {
      setCaret(caret.x, caret.y, selecting);
   }

   void setOverwriteMode(bool overwrite)
   {
      status.overwriteMode = overwrite;
   }
   void setReadOnlyMode(bool mode)
   {
      status.readOnly = mode;
   }
   void setHighlightMode(bool enabled)
   {
      if (_styler)
         _styler->setEnabled(enabled);
   }

   void moveLeft(bool selecting);
   void moveRight(bool selecting);
   void moveUp(bool selecting);
   void moveDown(bool selecting);
   void moveHome(bool selecting);
   void moveEnd(bool selecting);
   void moveFirst(bool selecting);
   void moveLast(bool selecting);

   void moveLeftToken(bool selecting);
   void moveRightToken(bool selecting, bool trimWhitespace = false);
   void moveFrameUp();
   void moveFrameDown();
   void moveToFrame(int column, int row, bool selecting);
   void movePageUp(bool selecting);
   void movePageDown(bool selecting);

   void selectWord();

   virtual void vscroll(int offset);
   virtual void hscroll(int offset);

   virtual void resize(Point size);

   void copySelection(text_c* text);
   void copyText(text_c* text, int length);

   void insertLine(text_t text, disp_t length);
   virtual void insertNewLine();
   void insertChar(text_c ch)
   {
      insertChar(ch, 1);
   }
   void insertChar(text_c ch, size_t number);

   void eraseChar(bool moveback);

   virtual void tabbing(text_c space, size_t count, bool indent);

   void trim();
   void duplicateLine();
   void eraseLine();
   void toLowercase();
   void toUppercase();
   void swap();

   void commentBlock();
   void uncommentBlock();

   virtual void undo();
   virtual void redo();

   void save(_ELENA_::path_t path);

   bool addMarker(HighlightInfo info, _ELENA_::pos_t bandStyle, _ELENA_::pos_t style, bool withCursor = true)
   {
      if (info.col == 0) {
         info.col = retrieveColumn(info.row, info.disp);
      }

      if (withCursor)
         setCaret(info, false);

      if (_styler && _styler->addMarker(info, bandStyle, style)) {
         status.frameChanged = true;

         return true;
      }
      else return false;
   }

   void removeMarker(int row, _ELENA_::pos_t bandStyle)
   {
      if (_styler) {
         _styler->removeMarker(row, bandStyle);

         status.frameChanged = true;
      }
   }

   Document(Text* text, LexicalStyler* styler, int encoding);
   virtual ~Document();
};

} // _GUI_

#endif // documentH
