//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Document class header         
//                                              (C)2005-2012, by Alexei Rakov
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
   size_t       step;
   size_t       style;

   int          row;
   TextBookmark bm;

   bool         bandStyle;   
   bool         newLine;
   bool         marker;
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
   _text_t _lookaheadState;
   _text_t _startState;

   // Lexical parser functions
   _text_t(* _makeStep)   (_text_t ch, _text_t state);
   size_t (* _defineStyle)(_text_t state, size_t style);

   int retrievePosition(size_t position)
   {
      position = (position >> INDEX_ORDER);

      _ELENA_::MemoryReader reader(&_index, position * 4);

      return reader.getDWord();
   }

   virtual void onUpdate(size_t position)
   {
      parse();
   }

   virtual void onInsert(size_t, size_t, const _text_t* )
   {
   }

   virtual void onErase(size_t, size_t, const _text_t*)
   {
   }

public:
   void setEnabled(bool enabled);

   void parse();

   virtual size_t proceed(size_t position, LexicalInfo& info);

   virtual bool addMarker(HighlightInfo info, int bandStyle, int style)
   {
      return false;
   }
   virtual void removeMarker(int row, int bandStyle)
   {
   }

   LexicalStyler(Text* text, int defaultStyle, _text_t lookaheadState, _text_t startState, 
      _text_t(* makeStep)(_text_t ch, _text_t state), size_t(* defineStyle)(_text_t state, size_t style));
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
   int           _selection; 

   size_t        _maxColumn;

   virtual void onUpdate(size_t position);
   virtual void onInsert(size_t position, size_t length, const _text_t* line);
   virtual void onErase(size_t position, size_t length, const _text_t* line);

   int defineStyle(Reader& reader);

   bool eraseSelection();

   int retrieveColumn(int row, int disp);

public:
   DocStatus status;

   bool hasSelection() const { return (_selection != 0); }
   int getSelectionLength();

   Text* getText() const { return _text; }
   TextBookmark getCurrentTextBookmark() { return _caret; }

   Point getFrame() const { return _frame.getCaret(); }
   Point getCaret(bool _virtual = true) const { return _caret.getCaret(_virtual); }
   Point getSize() const { return _size; }

   int getRowCount() const { return _text->getRowCount(); }
   int getMaxColumn() const { return _maxColumn; }

   bool findLine(const _text_t* text, bool matchCase, bool wholeWord);

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
   void moveToFrame(size_t column, size_t row, bool selecting);
   void movePageUp(bool selecting);
   void movePageDown(bool selecting);

   void selectWord();

   virtual void vscroll(int offset);
   virtual void hscroll(int offset);

   virtual void resize(Point size);

   void copySelection(_text_t* text);
   void copyText(_text_t* text, int length);

   void insertLine(const _text_t* text, int length);
   virtual void insertNewLine();
   void insertChar(_text_t ch)
   {
      insertChar(ch, 1);
   }
   void insertChar(_text_t ch, int number);

   void eraseChar(bool moveback);

   virtual void tabbing(_text_t space, size_t count, bool indent);

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

   void save(const _path_t* path);

   bool addMarker(HighlightInfo info, int bandStyle, int style, bool withCursor = true)
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

   void removeMarker(int row, int bandStyle)
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
