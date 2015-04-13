//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK Common Window Implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtkcommon.h"

using namespace _GUI_;

//// --- Callbacks ---
//
//static gboolean delete_event(GtkWidget *widget, GdkEvent  *event, gpointer data)
//{
//   return ((Window*)data)->on_Close() ? TRUE : FALSE;
//}
//
//static void frame_callback(GtkWidget *window, GdkEvent *event, gpointer data)
//{
//   ((Window*)data)->onResize(event->configure.x, event->configure.y, window->allocation.width, window->allocation.height);
//}
//
//// --- Control ---
//
//bool Control :: isVisible()
//{
//   return gtk_widget_get_visible(_handle);
//}
//
//void Control :: show()
//{
//   gtk_widget_show(_handle);
//}
//
//void Control :: hide()
//{
//   gtk_widget_hide_all(_handle);
//}
//
//void Control :: setFocus()
//{
//   gtk_widget_grab_focus(_handle);
//}
//
//void Control :: refresh()
//{
//   gtk_widget_queue_draw(_handle);
//}
//
//void Control :: setCaption(const char* caption)
//{
//   gtk_window_set_title(GTK_WINDOW(_handle), caption);
//}
//
//// --- Window ---
//
//void Window :: _initCallbacks()
//{
//    g_signal_connect(G_OBJECT(_handle), "delete_event",
//		      G_CALLBACK (delete_event), this);
//}
//
//void Window :: _initResizeCallback()
//{
//   gtk_widget_add_events(GTK_WIDGET(_handle), GDK_CONFIGURE);
//
//   g_signal_connect(G_OBJECT(_handle), "configure-event",
//        G_CALLBACK(frame_callback), this);
//}

// --- Clipboard ---

void Clipboard :: settext(const char* text)
{
  GtkClipboard *c = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  if (!c)
     return;

   gtk_clipboard_set_text(gtk_clipboard_get(GDK_SELECTION_CLIPBOARD), text, _ELENA_::getlength(text));
   gtk_clipboard_store(c);
}

char* Clipboard :: gettext()
{
  GtkClipboard *c = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
  if (!c)
     return NULL;

  if (!gtk_clipboard_wait_is_text_available(c))
     return NULL;

  return gtk_clipboard_wait_for_text(c);
}

void Clipboard :: freetext(char* text)
{
   g_free(text);
}

// --- DateTime ---

DateTime DateTime :: getFileTime(const char* path)
{
   DateTime dt;
   stat(path, &dt._time);

   return dt;
}
