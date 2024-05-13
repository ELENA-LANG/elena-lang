//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK+ TabBar Header File
//                                             (C)2024, by Aleksey Rakov
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
      int addTab(const char* name, Gtk::Widget* control);

      TabBar();
   };
}

#endif
