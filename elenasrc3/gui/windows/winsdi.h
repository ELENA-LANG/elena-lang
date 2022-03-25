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
   // --- LayoutManager ---
   class LayoutManager
   {
      ControlBase* _top;
      ControlBase* _left;
      ControlBase* _right;
      ControlBase* _bottom;
      ControlBase* _center;

   public:
      void setTop(ControlBase* top)
      {
         _top = top;
      }

      void setCenter(ControlBase* center)
      {
         _center = center;
      }

      void setBottom(ControlBase* bottom)
      {
         _bottom = bottom;
      }

      void setLeft(ControlBase* left)
      {
         _left = left;
      }

      void setRight(ControlBase* right)
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
      size_t        _childCounter;
      ControlBase** _children;
      LayoutManager _layoutManager;

      //void drawControls(HDC& hdc);

      void onResize() override;
      virtual void onActivate();
      void onDrawItem(DRAWITEMSTRUCT* item) override;
      virtual bool onCommand(int command) { return false; }

      LRESULT proceed(UINT message, WPARAM wParam, LPARAM lParam) override;

   public:
      static void registerSDIWindow(HINSTANCE hInstance, wstr_t className, HICON icon, wstr_t menuName, HICON smallIcon);

      void populate(size_t counter, ControlBase** children)
      {
         _children = new ControlBase * [counter];
         for (size_t i = 0; i < counter; i++) {
            _children[i] = children[i];
         }

         _childCounter = counter;
      }
      void setLayout(int center, int top, int bottom, int right, int left);

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