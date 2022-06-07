//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK+ TabBar Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------


#include "gtktabbar.h"

using namespace elena_lang;

// --- TabBar ---

TabBar::TabBar()
   : Gtk::Notebook()
{
}

int TabBar :: addTab(const char* name, Gtk::Widget* control)
{
   if (!get_visible())
      show();

   Gtk::HBox* hb = new Gtk::HBox(TRUE, 0);
   hb->pack_start(*control, TRUE, TRUE, 0);
   hb->show_all();

   append_page(*hb, name); // !! temporal
}
