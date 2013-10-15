//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK Toolbar Implementation
//                                              (C)2005-2010, by Alexei Rakov
//---------------------------------------------------------------------------
/*
#include "gtktoolbar.h"
#include "gtksdi.h"

using namespace _GUI_;

static void toolbar_clicked(GtkWidget *widget,  gpointer toolbar)
{
    ((ToolBar*)toolbar)->_onClick(widget);
}

// --- ToolBar ---

ToolBar :: ToolBar(Window* owner, int iconSize, int count, ToolBarButton* buttons)
{
   _owner = owner;
   _handle = gtk_toolbar_new();
   gtk_toolbar_set_style(GTK_TOOLBAR(_handle), GTK_TOOLBAR_ICONS);

   gtk_container_set_border_width(GTK_CONTAINER(_handle), 2);

   for (int i = 0 ; i < count ; i++) {
      if (buttons[i].optionId != 0) {
         GtkToolItem* button = gtk_tool_button_new_from_stock(buttons[i].iconId);
         gtk_toolbar_insert(GTK_TOOLBAR(_handle), button, -1);

         _options.add((size_t)button, buttons[i].optionId);

         g_signal_connect(G_OBJECT(button), "clicked",
            G_CALLBACK(toolbar_clicked), this);
      }
      else gtk_toolbar_insert(GTK_TOOLBAR(_handle), gtk_separator_tool_item_new(), -1);
   }
}

void ToolBar :: _onClick(GtkWidget* widget)
{
   int option = _options.get((size_t)widget);
   if (option != 0)
      ((SDIWindow*)_owner)->_onMenu(option);
}

void ToolBar :: enableItemById(int id, bool enabled)
{
   _ELENA_::Map<size_t, int>::Iterator it = _options.start();
   while (!it.Eof()) {
      if (*it == id) {
         gtk_widget_set_sensitive(GTK_WIDGET(it.key()), enabled ? TRUE : FALSE);
         gtk_widget_queue_draw(GTK_WIDGET(it.key()));

         break;
      }
      it++;
   }
}
*/