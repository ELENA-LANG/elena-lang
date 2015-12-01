//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK SDI Control Implementation File
//                                               (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtksdi.h"
//#include "gtkmenu.h"

using namespace _GUI_;

// --- SDIWindow ---

SDIWindow :: SDIWindow(const char* caption)
   : Gtk::Window(Gtk::WINDOW_TOPLEVEL), _box(Gtk::ORIENTATION_VERTICAL),
     _hbox(Gtk::ORIENTATION_HORIZONTAL), _vbox(Gtk::ORIENTATION_VERTICAL)
{
   set_title(caption);

   add(_box);

   _refUIManager = Gtk::UIManager::create();
   _refActionGroup = Gtk::ActionGroup::create();

   _refUIManager->insert_action_group(_refActionGroup);

   add_accel_group(_refUIManager->get_accel_group());
}

void SDIWindow :: loadUI(Glib::ustring ui_info, const char* name)
{
   _refUIManager->add_ui_from_string(ui_info);

   Gtk::Widget* pMenubar = _refUIManager->get_widget(name);
   if(pMenubar)
      _box.pack_start(*pMenubar, Gtk::PACK_SHRINK);
}

//void SDIWindow :: show(bool maximized)
//{
//   if (maximized)
//      gtk_window_maximize(GTK_WINDOW(_handle));
//
//   gtk_widget_show(_handle);
//   gtk_widget_show(_vbox);
//}

void SDIWindow :: exit()
{
   Gtk::Main::quit();
}

//void SDIWindow :: _initVPane()
//{
//   _vpaned = gtk_vpaned_new();
//   gtk_box_pack_start(GTK_BOX(_vbox), _vpaned, TRUE, TRUE, 0);
//   //gtk_paned_set_position(GTK_PANED(_vpaned), 0);
//
//   gtk_widget_show(_vpaned);
//}

void SDIWindow :: populate(Gtk::Widget* client, Gtk::Widget* left, Gtk::Widget* bottom, Gtk::Widget* statusbar)
{
   _box.pack_start(_hbox, TRUE, TRUE, 0);

   if (left)
      _hbox.pack_start(*left, Gtk::PACK_SHRINK);

   _hbox.pack_start(_vbox, Gtk::PACK_EXPAND_WIDGET);

   if (client)
      _vbox.pack_start(*client, TRUE, TRUE, 0);

   if (bottom)
      _vbox.pack_start(*bottom, Gtk::PACK_SHRINK);

   if (statusbar)
      _box.pack_start(*statusbar, Gtk::PACK_SHRINK);

   show_all_children(); // !!temporal
}

//void SDIWindow :: _addBottom(Control* control)
//{
//   GtkWidget* widget = (GtkWidget*)control->getHandle();
//
//   gtk_box_pack_end(GTK_BOX(_vbox), widget, FALSE, TRUE, 0);
//
//   gtk_widget_show_all(widget);
//}

