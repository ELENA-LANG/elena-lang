//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK SDI Control Header File
//                                             (C)2024-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKSDI_H
#define GTKSDI_H

#include "gtkcommon.h"

namespace elena_lang
{

// --- SDIWindow ---

class SDIWindow : public Gtk::ApplicationWindow
{
protected:
   Gtk::Box      _box;

   Gtk::Box      _tbox;
   Gtk::Box      _hbox;
   Gtk::Box      _bbox;

   Glib::RefPtr<Gtk::Builder>       _refBuilder;

   int                              _childCounter;
   Gtk::Widget**                    _children;

   bool _skip; // HOTFIX : to prevent infinite checkmenuitem call

   void loadUI(Glib::ustring ui_info, const char* name);

   bool toggleVisibility(int childIndex);

   //virtual Glib::RefPtr<Gtk::Action> getMenuItem(ustr_t name) = 0;

   virtual void checkMenuItemById(Glib::RefPtr<Gio::SimpleAction>& action, bool doEnable)
   {
      action->change_state(doEnable);
   }

   virtual void enableMenuItemById(Glib::RefPtr<Gio::SimpleAction>& action, bool doEnable)
   {
      action->set_enabled(doEnable);
   }

public:
   void populate(int counter, Gtk::Widget** children);
   void setLayout(int center, int top, int bottom, int right, int left);

   SDIWindow();
};

} // _GUI_

#endif // gtksdiH
