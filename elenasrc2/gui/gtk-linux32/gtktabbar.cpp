//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK Tabbar Implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtktabbar.h"

using namespace _GUI_;

// --- TabBar ---

TabBar :: TabBar()
   : Gtk::Notebook(), _tabs(NULL, _ELENA_::freeobj)
{
   set_scrollable(TRUE);
   _current = NULL;
}

int TabBar :: getCount()
{
   return _tabs.Count();
}

Gtk::Widget* TabBar :: _getTabControl(int index) const
{
   if (index == -1)
      return NULL;

   TabPages::Iterator it = _tabs.start();
   while (index > 0) {
      index--;
      it++;
   }
   return (*it)->control;
}

//const TCHAR* TabBar :: getTabName(int index)
//{
//   TabPages::Iterator it = _tabs.start();
//   while (!it.Eof()) {
//      if (index==0)
//         return it.key();
//
//      index--;
//      it++;
//   }
//   return NULL;
//}

int TabBar :: getTabIndex(text_str name)
{
   int index = 0;
   TabPages::Iterator it = _tabs.start();
   while (!it.Eof()) {
      if (name.compare(it.key()))
         return index;

      index++;
      it++;
   }
   return -1;
}

void TabBar :: eraseTabPage(int index)
{
   TabPages::Iterator it = _tabs.start();
   while (!it.Eof()) {
      if (index==0) {
         _tabs.erase(it);

         return;
      }

      index--;
      it++;
   }
}

int TabBar :: addTab(const char* name, Gtk::Widget* control)
{
   TabPage* page = new TabPage(name);
   page->control = control;

   _tabs.add(name, page);

   if (!get_visible())
      show();

   Gtk::HBox* hb = new Gtk::HBox(TRUE, 0);
   hb->pack_start(*control, TRUE, TRUE, 0);
   hb->show_all();

   append_page(*hb, name); // !! temporal

   int index = _tabs.Count() - 1;
   selectTab(index);

   return index;
}

void TabBar :: selectTab(int index)
{
   set_current_page(index);
}

void TabBar :: setFocus()
{
   if (!_current)
      _current = _getTabControl(getCurrentIndex());

   if (_current && _current->get_visible()) {
      _current->grab_focus();
   }
}

void TabBar :: onTabChange(int index)
{
   _current = _getTabControl(index);
}

void TabBar :: deleteTab(int index)
{
   _current = NULL;
   eraseTabPage(index);

   remove_page(index);

   if (_tabs.Count() == 0)
      hide();
}

//void TabBar :: renameTab(int index, const TCHAR* newName)
//{
//   // rename tab in the map
//   TabPages::Iterator it = _tabs.start();
//   for (int i = 0 ; i < index ; i++) {
//      it++;
//   }
//   it.rename(newName);
//
//   // rename caption
//   gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(_handle), gtk_notebook_get_nth_page(GTK_NOTEBOOK(_handle), index), newName);
//}
//
//void TabBar :: renameTabCaption(int index, const TCHAR* newName)
//{
//   // rename caption
//   gtk_notebook_set_tab_label_text(GTK_NOTEBOOK(_handle), gtk_notebook_get_nth_page(GTK_NOTEBOOK(_handle), index), newName);
//}

