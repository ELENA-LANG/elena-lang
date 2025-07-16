//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK+ TabBar Header File
//                                             (C)2024-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKTABBAR_H
#define GTKTABBAR_H

#include "gtkcommon.h"

namespace elena_lang
{
   // --- TabBar ---
   class TabBar : public Gtk::Notebook
   {
   protected:
      void on_switch_page(Widget* page, guint page_num) override
      {
         Gtk::Notebook::on_switch_page(page, page_num);

         onTabChange(page_num);
      }

   public:
      virtual void onTabChange(int page_num);

      void addTab(const char* name, Gtk::Widget* control);

      void selectTab(int index);

      int getCurrentTabIndex()
      {
         return get_current_page();
      }

      void renameTab(int index, const char* title);

      void deleteTab(int index);

      Gtk::Widget* getCurrentControl();

      TabBar();
   };
}

#endif
