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
}

void TabBar :: addTab(const char* name, Gtk::Widget* control)
{
   if (!get_visible())
      show();

   Gtk::HBox* hb = new Gtk::HBox(TRUE, 0);
   hb->pack_start(*control, TRUE, TRUE, 0);
   hb->show_all();

   append_page(*hb, name); // !! temporal
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

