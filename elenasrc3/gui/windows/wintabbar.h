//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TabBar Header File
//                                             (C)2021-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINTABBAR_H
#define WINTABBAR_H

#include "wincommon.h"

namespace elena_lang
{
   // --- CustomTabBar ---
   class CustomTabBar : public ControlBase
   {
   public:
      typedef void(*SelectionEventInvoker)(NotifierBase*, int);

   protected:
      NotifierBase*           _notifier;
      SelectionEventInvoker   _selectionInvoker;

      bool                    _withAbovescore;
      bool                    _notSelected;
      //bool                    _highlighted;

      HIMAGELIST              _hImages;

   public:
      bool isOverButton(Point p);

      void onDrawItem(DRAWITEMSTRUCT* item) override;

      int getCurrentIndex();
      int getTabCount();

      void addTab(int index, wstr_t title, void* param, int iconIndex = 0);
      void selectTab(int index);
      void renameTab(int index, wstr_t title);
      void deleteTab(int index);

      //bool highlightButton()
      //{
      //   if (!_highlighted) {
      //      _highlighted = true;

      //      return true;
      //   }
      //   return false;
      //}

      //bool clearButton()
      //{
      //   if (_highlighted) {
      //      _highlighted = false;

      //      return true;
      //   }
      //   return false;
      //}

      CustomTabBar(NotifierBase* notifier, bool withAbovescore, int width, int height);
      virtual ~CustomTabBar();
   };

   // --- MultiTabControl ---
   class MultiTabControl : public CustomTabBar
   {
   protected:
      ControlBase* _child;

      int          _iconId;

      void onSetFocus() override;

   public:
      void show() override;

      void setRectangle(Rectangle rec) override;

      int addTabView(wstr_t title, void* param);
      void renameTabView(int index, wstr_t title);
      void eraseTabView(int index);

      void setFocus() override;
      void refresh() override;

      HWND createControl(HINSTANCE instance, ControlBase* owner);

      MultiTabControl(NotifierBase* notifier, bool withAbovescore, ControlBase* child, int iconId = 0);
   };

   // --- TabBar ---
   class TabBar : public CustomTabBar
   {
      ControlBase*       _current;
      List<ControlBase*> _pages;

      void resizeTab(Rectangle* clientRect, ControlBase* control);
      void showCurrentTab();

   public:
      HWND createControl(HINSTANCE instance, ControlBase* owner);

      void addTabChild(const wchar_t* name, ControlBase* window);
      void removeTabChild(ControlBase* window);

      bool selectTabChild(ControlBase* window);

      void setRectangle(Rectangle rec) override;

      void onSelChanged() override
      {
         showCurrentTab();
      }

      bool empty()
      {
         return _pages.count() == 0;
      }

      void refresh() override;
      void invalidate() override;

      TabBar(NotifierBase* notifier, bool withAbovescore, int height);
   };

}

#endif