//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK SDI Control Header File
//                                               (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef gtksdiH
#define gtksdiH

#include "gtkcommon.h"

namespace _GUI_
{

// --- SDIWindow ---

class SDIWindow : public Gtk::Window
{
protected:
   Gtk::Box                   _box;
   Gtk::Box                   _hbox;
   Gtk::Box                   _vbox;

   Glib::RefPtr<Gtk::UIManager>   _refUIManager;
   Glib::RefPtr<Gtk::ActionGroup> _refActionGroup;
//   Gtk::VPaned _vpaned;

//   void _addBottom(Control* control);

   void loadUI(Glib::ustring ui_info, const char* name);

public:
   virtual void populate(Gtk::Widget* client, Gtk::Widget* left, Gtk::Widget* bottom, Gtk::Widget* statusbar);

   virtual void exit();

//   virtual void show() { Control::show(); }
//   virtual void show(bool maximized);
//
//   void _onMenu(int optionID)
//   {
//      _feedback->onMenu(optionID);
//   }

   SDIWindow(const char* caption);
};

} // _GUI_

#endif // gtksdiH
