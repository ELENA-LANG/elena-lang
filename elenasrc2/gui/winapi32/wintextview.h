//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TextView Control Header File
//                                               (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef wintextviewH
#define wintextviewH

#include "wincommon.h"
#include "wingraphic.h"
#include "document.h"

namespace _GUI_
{

// --- Notification messages ---
#define TEXT_VIEW_CHANGED                      100
#define CONTEXT_MENU_ON                        101
#define TEXT_VIEW_MARGINCLICKED                102

#define EDIT_WND_CLASS                          _T("ELENA EDITOR CLASS")

// --- StyleInfo ---

struct StyleInfo
{
   Colour       foreground;
   Colour       background;
   const wchar_t* faceName;
   int          characterSet;
   int          size;
   bool         bold;
   bool         italic;
};

// --- ViewStyle ---

class ViewStyles
{
   Style* _styles;
   size_t _count;

   int   _lineHeight;
   int   _marginWidth;

public:
   size_t Count() const { return _count; }

   Style operator[](size_t index)
   {
      return _styles[index];
   }

   void assign(size_t count, StyleInfo* styles, int lineHeight, int marginWidth);

   int getLineHeight() const { return _lineHeight; }
   int getMarginWidth() const { return _marginWidth; }

   void validate(Canvas* canvas);

   void release();

   ViewStyles() { _marginWidth = _lineHeight = 0; _count = 0; }
   ~ViewStyles() { release(); }
};

// --- TextView ---

class TextView : public Window
{
protected:
   Control*    _receptor;

   Canvas      _zbuffer;
   bool        _cached;
   bool        _caretValid;
   bool        _caretVisible;
   bool        _mouseCaptured;

   ViewStyles  _styles;
   bool        _lineNumbersVisible;
   bool        _highlight;

   Document*   _document;
   bool        _needToResize;
   bool        _tabUsing;
   int         _tabSize;
   int         _caret_x;

   Document::Reader* _docReader;

   virtual LRESULT _WindProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

   void paint(Canvas& canvas, _GUI_::Rectangle clientRect);

   void createCaret(int height, int width);
   void destroyCaret();
   void showCaret();
   void hideCaret();
   void _locateCaret(int x, int y);
   void _captureMouse();
   void _releaseMouse();
   bool _isMouseCaptured() { return _mouseCaptured; }

   virtual void onPaint();
   virtual bool onKeyDown(int keyCode, bool kbShift, bool kbCtrl);
   virtual bool onKeyPressed(wchar_t ch);
   virtual void onScroll(int bar, int type);
   virtual bool _onSetCursor();
   virtual void onResize();
   virtual void onSetFocus();
   virtual void onLoseFocus();
   virtual void _onButtonUp();
   virtual void _onButtonDown(Point point, bool kbShift);
   virtual void _onMouseMove(Point point, bool kbLButton);
   virtual void _onMouseWheel(short wheelDelta, bool kbCtrl);
   virtual void _onDoubleClick();
   virtual void _onContextMenu(HWND, short x, short y);

   void onEditorChange(/*bool firstChange*/);

   void update(bool resize = true);
   void updateVScroller(bool resize);
   void updateHScroller(bool resize);

   void resizeDocument();

   void mouseToScreen(Point point, int& col, int& row, bool& margin);

   int getHScrollerPosition();
   int getVScrollerPosition();
   int getScrollerPosition(int bar)
   {
      return (bar == SB_VERT) ? getVScrollerPosition() : getHScrollerPosition();
   }

   int getLineNumberMargin();

   void _setScrollPosition(int bar, int position);   

   bool _getScrollInfo(int bar, SCROLLINFO* info);
   void _setScrollInfo(int bar, int max, int page);

public:
   static void _registerClass(HINSTANCE instance);

   void setReceptor(Control* receptor)
   {
      _receptor = receptor;
   }

   bool isCursorVisible() const { return _caretVisible; }

   virtual Document* getDocument() { return _document; }

   virtual void setDocument(Document* document);
   virtual void setStyles(int count, StyleInfo* styles, int lineHeight, int marginWidth);

   void indent();
   void outdent();

   void applySettings(int tabSize, bool tabUsing, bool lineNumberVisible, bool highlight);

   virtual void refreshView()
   {
      onEditorChange();
   }

   TextView(Control* owner, int left, int top, int width, int height);
};

} // _GUI_

#endif // wintextviewH