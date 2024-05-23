//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK SDI Control Header File
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKSDI_H
#define GTKSDI_H

#include "gtkcommon.h"

namespace elena_lang
{

// --- SDIWindow ---

class SDIWindow : public Gtk::Window
{
protected:
   Gtk::Box      _box;
   Gtk::Box      _hbox;
   Gtk::Box      _vbox;

   Glib::RefPtr<Gtk::UIManager>   _refUIManager;
   Glib::RefPtr<Gtk::ActionGroup> _refActionGroup;

   int           _childCounter;
   Gtk::Widget** _children;

   void loadUI(Glib::ustring ui_info, const char* name);

public:
   void populate(int counter, Gtk::Widget** children);
   void setLayout(int center, int top, int bottom, int right, int left);

   SDIWindow();
};

} // _GUI_

#endif // gtksdiH
