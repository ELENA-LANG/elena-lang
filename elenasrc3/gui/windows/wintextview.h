//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TextView Control Header File
//                                               (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINTEXTVIEW_H
#define WINTEXTVIEW_H

#include "wincommon.h"
#include "guieditor.h"
#include "wincanvas.h"

namespace elena_lang
{
   class TextViewWindow : public WindowBase, public DocumentNotifier
   {
   protected:
      TextViewModelBase*      _model;
      TextViewControllerBase* _controller;

      bool                    _cached;
      bool                    _needToResize;
      bool                    _caretValid;
      bool                    _caretVisible;
      bool                    _mouseCaptured;

      int                     _caret_x;
      Canvas                  _zbuffer;

      int getLineNumberMargin();

      void resizeDocument();

      void mouseToScreen(Point point, int& col, int& row, bool& margin);

      void createCaret(int height, int width);
      void destroyCaret();
      void showCaret();
      void hideCaret();
      void locateCaret(int x, int y);

      void update(bool resize = true);

      void paint(Canvas& canvas, Rectangle clientRect);

      void captureMouse();
      void releaseMouse();

      void onButtonDown(Point point, bool kbShift);
      void onButtonUp();

      bool onKeyDown(int keyCode, bool kbShift, bool kbCtrl);
      void onPaint();
      void onResize() override;
      void onSetFocus() override;
      void onLoseFocus() override;
      bool onSetCursor() override;

   public:
      void onDocumentUpdate() override;

      static void registerTextViewWindow(HINSTANCE hInstance, wstr_t className);

      HWND create(HINSTANCE instance, wstr_t className, ControlBase* owner) override;

      LRESULT proceed(UINT message, WPARAM wParam, LPARAM lParam) override;

      TextViewWindow(TextViewModelBase* model, TextViewControllerBase* controller);
      virtual ~TextViewWindow();
   };
}

#endif