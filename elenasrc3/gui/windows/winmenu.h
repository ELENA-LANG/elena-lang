//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Menu Header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINMENU_H
#define WINMENU_H

#include "wincommon.h"

namespace  elena_lang
{
   // --- Menu ---
   class MenuBase : public GUIControlBase
   {
   protected:
      HMENU  _handle;

   public:
      bool checkHandle(void* param) const override
      {
         return _handle == (HMENU)param;
      }

      Rectangle getRectangle() override { return { }; }
      void setRectangle(Rectangle rec) override {}

      void show() override {}
      void hide() override {}
      bool visible() override { return true; }

      void setFocus() override {}
      void refresh() override {}

      void checkItemById(int id, bool checked);
   };

   class RootMenu : public MenuBase
   {
   public:
      RootMenu(HMENU hMenu);
   };
}

#endif
