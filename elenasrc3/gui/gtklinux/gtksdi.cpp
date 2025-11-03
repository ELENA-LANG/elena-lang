//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK SDI Control Implementation File
//                                             (C)2024-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtksdi.h"

using namespace elena_lang;

// --- SDIWindow ---

SDIWindow :: SDIWindow()
   : Gtk::ApplicationWindow(/*Gtk::WINDOW_TOPLEVEL*/), _box(Gtk::Orientation::VERTICAL),
     _tbox(Gtk::Orientation::VERTICAL),
     _hbox(Gtk::Orientation::HORIZONTAL), _bbox(Gtk::Orientation::VERTICAL)
{
   _children = nullptr;
   _childCounter = 0;
   _skip = false;

   _box.append(_tbox);
   _box.append(_hbox);
   _box.append(_bbox);

   set_child(_box);

   _refBuilder = Gtk::Builder::create();
}

void SDIWindow :: populate(int counter, Gtk::Widget** children)
{
   _children = children;
   _childCounter = counter;
}

void SDIWindow::setLayout(int client, int top, int bottom, int right, int left)
{
   if (left != -1)
      //_hbox.pack_start(*_children[left], Gtk::PACK_SHRINK);
      _hbox.append(*_children[left]);

   if (client != -1) {
      //_vbox.pack_start(*_children[client], TRUE, TRUE, 0);
      _hbox.append(*_children[client]);
      _children[client]->set_hexpand(true);
      _children[client]->set_vexpand(true);
   }

   if (bottom != -1)
      //_vbox.pack_start(*_children[bottom], Gtk::PACK_SHRINK);
      _bbox.append(*_children[bottom]);

//   if (statusbar)
//      _box.pack_start(*statusbar, Gtk::PACK_SHRINK);
}

void SDIWindow :: loadUI(Glib::ustring ui_info, const char* name)
{
   _refBuilder->add_from_string(ui_info);

   auto gmenu = _refBuilder->get_object<Gio::Menu>(name);
   if(gmenu) {
      auto menuBar = Gtk::make_managed<Gtk::PopoverMenuBar>(gmenu);

      _tbox.append(*menuBar);
   }
}

bool SDIWindow :: toggleVisibility(int childIndex)
{
   bool visible = _children[childIndex]->get_visible();

   visible = !visible;

   _children[childIndex]->set_visible(visible);

   return visible;
}
