//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK SDI Control Implementation File
//                                               (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtksdi.h"
//#include "gtkmenu.h"

using namespace _GUI_;

// --- SDIWindow ---

SDIWindow :: SDIWindow(const _text_t* caption)
   : Gtk::Window(Gtk::WINDOW_TOPLEVEL), _box(Gtk::ORIENTATION_VERTICAL)
{
   add(_box);

   _refActionGroup = Gtk::ActionGroup::create();
   _refUIManager = Gtk::UIManager::create();

   _refUIManager->insert_action_group(_refActionGroup);

   add_accel_group(_refUIManager->get_accel_group());

   set_title(caption);
}

void SDIWindow :: loadUI(Glib::ustring ui_info)
{
   _refUIManager->add_ui_from_string(ui_info);

   Gtk::Widget* pMenubar = _refUIManager->get_widget("/MenuBar");
   if(pMenubar)
      _box.pack_start(*pMenubar, Gtk::PACK_SHRINK);

//   _box.pack_start(_vpaned, Gtk::PACK_EXPAND_WIDGET);
//   _vpaned.set_position(0);
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

void SDIWindow :: populate(Gtk::Widget* client/*, Control* statusbar*/)
{
   if (client) {
      _box.pack_start(*client, TRUE, TRUE, 0);

      //_vpaned.add1(*client);
//   gtk_paned_pack1(GTK_PANED(_vpaned), widget, TRUE, TRUE);
//   //gtk_box_pack_start(GTK_BOX(_vbox), widget, TRUE, TRUE, 0);
   }

//   if (statusbar)
//      _addBottom((Control*)statusbar);

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

