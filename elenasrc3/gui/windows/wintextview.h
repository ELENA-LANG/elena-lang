//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TextView Control Header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINTEXTVIEW_H
#define WINTEXTVIEW_H

#include "wincommon.h"
#include "guieditor.h"
#include "wincanvas.h"

namespace elena_lang
{
   // --- StyleInfo ---
   struct StyleInfo
   {
      Color  foreground;
      Color  background;
      wstr_t faceName;
      int    characterSet;
      int    size;
      bool   bold;
      bool   italic;
   };

   // --- ViewStyles ---
   struct ViewStyles
   {
      Style* items;
      pos_t  count;

      int    marginWidth;
      int    lineHeight;

      void assign(pos_t count, StyleInfo* styles, int lineHeight, int marginWidth, FontFactory* factory);
      void release();

      Style* getStyle(pos_t index) const
      {
         return &items[index];
      }

      int getMarginWidth() const
      {
         return marginWidth;
      }

      int getLineHeight() const
      {
         return lineHeight;
      }

      void validate(Canvas* canvas)
      {
         for (pos_t i = 0; i < count; i++) {
            canvas->validateStyle(&items[i]);

            if (items[i].valid && lineHeight < items[i].lineHeight)
               lineHeight = items[i].lineHeight;
         }
      }

      ViewStyles()
      {
         lineHeight = marginWidth = 0;
         items = nullptr;
         count = 0;
      }
   };

   // --- TextViewWindow ---
   class TextViewWindow : public WindowBase, public DocumentNotifier
   {
   protected:
      TextViewModelBase*      _model;
      ViewStyles*             _styles;
      TextViewControllerBase* _controller;

      bool                    _cached;
      bool                    _needToResize;
      bool                    _caretValid;
      bool                    _caretVisible;
      bool                    _mouseCaptured;

      int                     _caret_x;
      Canvas                  _zbuffer;

      int getLineNumberMargin();

      int getVScrollerPosition();
      int getHScrollerPosition();
      int getScrollerPosition(int bar)
      {
         return (bar == SB_VERT) ? getVScrollerPosition() : getHScrollerPosition();
      }

      bool getScrollInfo(int bar, SCROLLINFO* info);

      void resizeDocument();

      void mouseToScreen(Point point, int& col, int& row, bool& margin);

      void createCaret(int height, int width);
      void destroyCaret();
      void showCaret();
      void hideCaret();
      void locateCaret(int x, int y);

      void setScrollPosition(int bar, int position);
      void setScrollInfo(int bar, int max, int page);

      void update(bool resize = true);
      void updateVScroller(bool resize);
      void updateHScroller(bool resize);

      void paint(Canvas& canvas, Rectangle clientRect);

      void captureMouse();
      void releaseMouse();

      void onButtonDown(Point point, bool kbShift);
      void onButtonUp();
      void onMouseMove(short wheelDelta, bool kbCtrl);

      bool onKeyDown(int keyCode, bool kbShift, bool kbCtrl);
      bool onKeyPressed(wchar_t ch);
      void onPaint();
      void onResize() override;
      void onSetFocus() override;
      void onLoseFocus() override;
      bool onSetCursor() override;

      void onScroll(int bar, int type);

   public:
      void onDocumentUpdate() override;

      void refresh() override;

      static void registerTextViewWindow(HINSTANCE hInstance, wstr_t className);

      HWND create(HINSTANCE instance, wstr_t className, ControlBase* owner) override;

      LRESULT proceed(UINT message, WPARAM wParam, LPARAM lParam) override;

      TextViewWindow(TextViewModelBase* model, TextViewControllerBase* controller, ViewStyles* styles);
      virtual ~TextViewWindow();
   };
}

#endif