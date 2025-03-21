//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK+ TabBar Header File
//                                             (C)2024-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKTABBAR_H
#define GTKTABBAR_H

#include "gtkcommon.h"

namespace elena_lang
{
   // --- TabBar ---
   class TabBar : public Gtk::Notebook
   {
   public:
      void addTab(const char* name, Gtk::Widget* control);

      Gtk::Widget* getCurrentControl();

      TabBar();
   };
}

#endif
