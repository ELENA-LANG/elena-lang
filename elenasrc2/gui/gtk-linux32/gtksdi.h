//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK SDI Control Header File
//                                               (C)2005-2010, by Alexei Rakov
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
   Glib::RefPtr<Gtk::UIManager>   _refUIManager;
   Glib::RefPtr<Gtk::ActionGroup> _refActionGroup;

   Gtk::Box _box;
//   Gtk::VPaned _vpaned;
//   //ToolBar* _toolBar;

//   void _addBottom(Control* control);

   void loadUI(Glib::ustring ui_info);

public:
   virtual void populate(Gtk::Widget* client/*, Control* statusbar*/);

   virtual void exit();

//   virtual void show() { Control::show(); }
//   virtual void show(bool maximized);
//
//   void _onMenu(int optionID)
//   {
//      _feedback->onMenu(optionID);
//   }
//
//   void _onClientChanged(void* data, int extraparam, int type)
//   {
//      _feedback->onClientChanged(data, extraparam, type);
//   }

   SDIWindow(const _text_t* caption);
};

} // _GUI_

#endif // gtksdiH
