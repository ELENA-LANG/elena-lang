//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TabBar Header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINTABBAR_H
#define WINTABBAR_H

#include "wincommon.h"

namespace elena_lang
{
   // --- CustomTabBar ---
   class CustomTabBar : public ControlBase
   {
   protected:
      NotifierBase*  _notifier;

      bool           _withAbovescore;
      bool           _notSelected;

   public:
      void onDrawItem(DRAWITEMSTRUCT* item) override;

      int getCurrentIndex();
      int getTabCount();

      void addTab(int index, wstr_t title, void* param);
      void selectTab(int index);
      void renameTab(int index, wstr_t title);
      void deleteTab(int index);

      CustomTabBar(NotifierBase* notifier, bool withAbovescore, int width, int height);
   };

   // --- MultiTabControl ---
   class MultiTabControl : public CustomTabBar
   {
   protected:
      ControlBase* _child;

      void onSetFocus() override;

   public:
      void show() override;

      void setRectangle(Rectangle rec) override;

      int addTabView(wstr_t title, void* param);
      void renameTabView(int index, wstr_t title);
      void eraseTabView(int index);

      void setFocus() override;

      HWND createControl(HINSTANCE instance, ControlBase* owner);

      MultiTabControl(NotifierBase* notifier, bool withAbovescore, ControlBase* child);
   };

   // --- TabBar ---
   class TabBar : public CustomTabBar
   {
      List<ControlBase*> _pages;

   public:
      HWND createControl(HINSTANCE instance, ControlBase* owner);

      void addTabChild(const wchar_t* name, ControlBase* window);
      void removeTabChild(ControlBase* window);

      void selectTabChild(ControlBase* window);

      void setRectangle(Rectangle rec) override;

      void onSelChanged() override
      {
         refresh();
      }

      void refresh() override;

      TabBar(NotifierBase* notifier, bool withAbovescore);
   };

}

#endif