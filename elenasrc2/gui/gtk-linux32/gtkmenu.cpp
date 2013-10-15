//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK+ Menu Window Implementation
//                                              (C)2005-2010, by Alexei Rakov
//---------------------------------------------------------------------------
/*
#include "gtkmenu.h"

using namespace _GUI_;

// --- Menu ---

Menu :: Menu(GtkItemFactoryEntry* items, int itemNumber, void* object)
{
   _accel = gtk_accel_group_new();
   _factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<main>", _accel);

   gtk_item_factory_create_items (_factory, itemNumber, items, object);
   _widget = gtk_item_factory_get_widget (_factory, "<main>");
}

void Menu :: enableItemById(int id, bool enabled)
{
   GtkWidget* item = gtk_item_factory_get_item_by_action(_factory, id);

   gtk_widget_set_sensitive(item, enabled ? TRUE : FALSE);
   gtk_widget_queue_draw(item);
}

void Menu :: checkItemById(int id, bool checked)
{
//   GtkWidget* item = gtk_item_factory_get_item_by_action(_factory, id);

//   if (checked && !gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item))) {
//      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), TRUE);
//   }
//   else if (!checked && gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item))) {
//      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), FALSE);
//   }
}

void Menu :: eraseItemById(int id)
{
   GtkWidget* item = gtk_item_factory_get_item_by_action(_factory, id);

   gtk_widget_hide_all(item);
}

void Menu :: insertItemById(int, int id, const TCHAR* caption)
{
   GtkWidget* item = gtk_item_factory_get_item_by_action(_factory, id);

   gtk_widget_show_all(item);
}

void Menu :: insertSeparatorById(int, int id)
{
   GtkWidget* item = gtk_item_factory_get_item_by_action(_factory, id);

   gtk_widget_show_all(item);
}

void Menu :: renameItemById(int id, const TCHAR* caption)
{
   GtkWidget* item = gtk_item_factory_get_item_by_action(_factory, id);

   gtk_menu_item_set_label(GTK_MENU_ITEM(item), caption);
}
*/