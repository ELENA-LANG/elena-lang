//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Menu Header File
//                                             (C)2021-2023, by Aleksey Rakov
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
      void enableMenuItemById(int id, bool enabled);
   };

   class RootMenu : public MenuBase
   {
   public:
      RootMenu(HMENU hMenu);
   };

   // --- ContextMenu ---

   struct MenuInfo
   {
      size_t         key;
      const wchar_t* text;
   };

   class ContextMenu : public MenuBase
   {
      bool isLoaded() const { return _handle != nullptr; }

   public:
      void create(int count, MenuInfo* items);

      void show(HWND parent, Point& p) const;

      ContextMenu();
      virtual ~ContextMenu();
   };


}

#endif
