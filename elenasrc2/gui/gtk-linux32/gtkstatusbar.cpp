//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK Statusbar Implementation
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtkstatusbar.h"

using namespace _GUI_;

// --- StatusBar ---

//StatusBar :: StatusBar(Window* owner, int partCount, int* widths)
//{
//   _handle = gtk_hbox_new (false, 3);
//
//   _items = new GtkWidget*[partCount];
//
//   for (int i = 0 ; i < partCount ; i++) {
//      GtkWidget* frm = gtk_frame_new("");
//
//      GtkWidget* lbl = gtk_label_new("");
//
//      _items[i] = lbl;
//
//      GtkWidget* align = gtk_alignment_new(0.0, 0.5, 0.0, 0.0);
//      gtk_container_add(GTK_CONTAINER(align), lbl);
//      gtk_container_add(GTK_CONTAINER(frm), align);
//
//      if (widths[i] != 0) {
//         gtk_widget_set_size_request(frm, widths[i], 10);
//
//         gtk_box_pack_start(GTK_BOX(_handle), frm, false, false, 0);
//      }
//      else gtk_box_pack_start(GTK_BOX(_handle), frm, true, true, 0);
//
//      gtk_widget_show (frm);
//   }
//}

bool StatusBar :: setText(int index, const char* str)
{
//   gtk_label_set_text(GTK_LABEL(_items[index]), str);

   return true;
}

//StatusBar :: ~StatusBar()
//{
//   if (_items != NULL)
//      delete[] _items;
//}

