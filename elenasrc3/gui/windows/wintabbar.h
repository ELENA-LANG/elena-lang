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

   public:
      int getTabCount();

      void addTab(int index, wstr_t title, void* param);

      CustomTabBar();
   };

   // --- MultiTabControl ---
   class MultiTabControl : public CustomTabBar
   {
   public:
      int addTabView(wstr_t title, void* param);

      HWND createControl(HINSTANCE instance, ControlBase* owner);

      MultiTabControl(ControlBase* child);
   };

}

#endif