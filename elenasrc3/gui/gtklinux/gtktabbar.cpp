//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK+ TabBar Implementation File
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------


#include "gtktabbar.h"

using namespace elena_lang;

// --- TabBar ---

TabBar::TabBar()
   //: Gtk::Widget()
{
   signal_switch_page().connect(sigc::mem_fun(*this,
              &TabBar::on_notebook_switch_page) );
}

void TabBar :: addTab(const char* name, Gtk::Widget* control)
{
   if (!get_visible())
      show();

   Gtk::Box* hb = new Gtk::Box();
   //hb->pack_start(*control, TRUE, TRUE, 0);
   hb->append(*control);
   //hb->show_all();

   append_page(*hb, name); // !! temporal
}

Gtk::Widget* TabBar :: getCurrentControl()
{
   Gtk::Box* hb = dynamic_cast<Gtk::Box*>(get_nth_page(get_current_page()));
   if (!hb)
      return nullptr;

   auto list =  hb->get_children();

   return list[0];
}

void TabBar :: selectTab(int index)
{
   set_current_page(index);
}

void TabBar :: onTabChange(int page_num)
{
}

void TabBar :: renameTab(int index, const char* title)
{
   set_tab_label_text(*get_nth_page(index), title);
}

void TabBar :: deleteTab(int index)
{
   remove_page(index);
}

/*Gtk::Widget* TabBar :: getTabControl(int index) const
{
   if (index == -1)
      return nullptr;

   TabPages::Iterator it = _tabs.start();
   while (index > 0) {
      index--;
      it++;
   }
   return (*it)->control;
}*/
