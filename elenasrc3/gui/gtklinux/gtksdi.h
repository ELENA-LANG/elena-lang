//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK SDI Control Header File
//                                             (C)2024-2025, by Aleksey Rakov
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

   virtual void checkMenuItemById(ustr_t name, bool doEnable)
   {
      /*Glib::RefPtr<Gtk::Action> menuItem = getMenuItem(name);

      Glib::RefPtr<Gtk::ToggleAction> toggleItem =
         Glib::RefPtr<Gtk::ToggleAction>::cast_static(menuItem);

      if (toggleItem->get_active() != doEnable) {
         _skip = true;
         toggleItem->set_active(doEnable);
      }*/
   }

public:
   void populate(int counter, Gtk::Widget** children);
   void setLayout(int center, int top, int bottom, int right, int left);

   SDIWindow();
};

} // _GUI_

#endif // gtksdiH
