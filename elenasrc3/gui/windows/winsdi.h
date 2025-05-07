//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI SDI Window Header File
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINSDI_H
#define WINSDI_H

#include "wincommon.h"

namespace elena_lang
{
   // --- BoxBase ---
   class BoxBase : public GUIControlBase
   {
   protected:
      CachedList<GUIControlBase*, 3> _list;
      bool                           _stretchMode;
      int                            _spacer;

   public:
      bool checkHandle(void* param) const override
      {
         return false;
      }

      void show() override;
      void hide() override;

      bool visible() override;

      void setFocus() override;

      void refresh() override;
      void invalidate() override;

      BoxBase(bool stretchMode, int spacer);
   };

   // --- VerticalBox ---
   class VerticalBox : public BoxBase
   {
   public:
      void append(GUIControlBase* item);

      Rectangle getRectangle() override;
      void setRectangle(Rectangle rec) override;

      VerticalBox(bool stretchMode, int spacer);
   };

   // --- HorizontalBox ---
   class HorizontalBox : public BoxBase
   {
   public:
      void append(GUIControlBase* item);

      Rectangle getRectangle() override;
      void setRectangle(Rectangle rec) override;

      HorizontalBox(bool stretchMode, int spacer);
   };

   // --- LayoutManager ---
   class LayoutManager
   {
      GUIControlBase* _top;
      GUIControlBase* _left;
      GUIControlBase* _right;
      GUIControlBase* _bottom;
      GUIControlBase* _center;

   public:
      GUIControlBase* getCenter()
      {
         return _center;
      }

      void setTop(GUIControlBase* top)
      {
         _top = top;
      }

      void setCenter(GUIControlBase* center)
      {
         _center = center;
      }

      void setBottom(GUIControlBase* bottom)
      {
         _bottom = bottom;
      }

      void setLeft(GUIControlBase* left)
      {
         _left = left;
      }

      void setRight(GUIControlBase* right)
      {
         _right = right;
      }

      void resizeTo(Rectangle area);

      LayoutManager()
      {
         _center = nullptr;
         _top = _bottom = nullptr;
         _left = _right = nullptr;
      }
   };

   // --- SDIWindow ---
   class SDIWindow : public WindowBase
   {
   protected:
      size_t           _childCounter;
      GUIControlBase** _children;
      LayoutManager    _layoutManager;

      HBRUSH           _bkBrush;

      //void drawControls(HDC& hdc);

      void onResize() override;
      virtual void onActivate();
      virtual void onResizing(RECT* rect);
      void onDrawItem(DRAWITEMSTRUCT* item) override;
      virtual bool onCommand(int command) { return false; }
      virtual void onNotify(NMHDR* hdr);
      bool onSetCursor() override;

      virtual void onDropFiles(HDROP hDrop) {}

      LRESULT proceed(UINT message, WPARAM wParam, LPARAM lParam) override;

      void setBackgroundColor(Color color);
      int paintBackground();

   public:
      static void registerSDIWindow(HINSTANCE hInstance, wstr_t className, HICON icon, wstr_t menuName, HICON smallIcon);

      virtual void populate(size_t counter, GUIControlBase** children)
      {
         _children = new GUIControlBase *[counter];
         for (size_t i = 0; i < counter; i++) {
            _children[i] = children[i];
         }

         _childCounter = counter;
      }
      void setLayout(int center, int top, int bottom, int right, int left);

      void close();

      void refresh() override
      {
         WindowBase::refresh();

         onResize();
      }

      virtual void exit();

      bool setColor(int, Color) override;

      SDIWindow(wstr_t title)
         : WindowBase(title, 800, 600)
      {
         _children = nullptr;
         _childCounter = 0;
         _bkBrush = nullptr;
      }
      ~SDIWindow() override
      {
         if (_childCounter != 0) {
            for (size_t i = 0; i < _childCounter; i++) {
               freeobj(_children[i]);
            }

            delete _children;
         }            
      }
   };

}

#endif