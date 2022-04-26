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
      bool     _withAbovescore;

   public:
      void onDrawItem(DRAWITEMSTRUCT* item) override;

      int getCurrentIndex();
      int getTabCount();

      void addTab(int index, wstr_t title, void* param);
      void selectTab(int index);

      CustomTabBar(bool withAbovescore);
   };

   // --- MultiTabControl ---
   class MultiTabControl : public CustomTabBar
   {
   protected:
      ControlBase* _child;

      void onSetFocus() override;

   public:
      void setRectangle(Rectangle rec) override;

      int addTabView(wstr_t title, void* param);

      void setFocus() override;

      HWND createControl(HINSTANCE instance, ControlBase* owner);

      MultiTabControl(bool withAbovescore, ControlBase* child);
   };

}

#endif