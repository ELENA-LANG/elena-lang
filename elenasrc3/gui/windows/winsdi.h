//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI SDI Window Header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINSDI_H
#define WINSDI_H

#include "wincommon.h"

namespace elena_lang
{
   // --- VerticalBox ---
   class VerticalBox : public GUIControlBase
   {
      CachedList<GUIControlBase*, 3> _list;
      bool                           _stretchMode;

   public:
      void append(GUIControlBase* item)
      {
         _list.add(item);
      }

      bool checkHandle(void* param) const override
      {
         return false;
      }

      Rectangle getRectangle() override;
      void setRectangle(Rectangle rec) override;

      void show() override;
      void hide() override;

      bool visible() override;

      void setFocus() override;

      VerticalBox(bool stretchMode);
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

      //void drawControls(HDC& hdc);

      void onResize() override;
      virtual void onActivate();
      void onDrawItem(DRAWITEMSTRUCT* item) override;
      virtual bool onCommand(int command) { return false; }
      virtual void onNotify(NMHDR* hdr);

      LRESULT proceed(UINT message, WPARAM wParam, LPARAM lParam) override;

   public:
      static void registerSDIWindow(HINSTANCE hInstance, wstr_t className, HICON icon, wstr_t menuName, HICON smallIcon);

      void populate(size_t counter, GUIControlBase** children)
      {
         _children = new GUIControlBase *[counter];
         for (size_t i = 0; i < counter; i++) {
            _children[i] = children[i];
         }

         _childCounter = counter;
      }
      void setLayout(int center, int top, int bottom, int right, int left);

      void close();

      SDIWindow(wstr_t title)
         : WindowBase(title)
      {
         _children = nullptr;
         _childCounter = 0;
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