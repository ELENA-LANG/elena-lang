//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK Common Window Implementation
//                                              (C)2024-2025, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtkcommon.h"

using namespace elena_lang;

// --- WindowApp ---

int WindowApp :: run(GUIControlBase* mainWindow, bool maximized, EventBase* startEvent)
{
   Gtk::Window* window = dynamic_cast<WindowWrapper*>(mainWindow)->getHandle();

//      if (maximized)
//         maximize();

   window->show();

   Gtk::Main::run(*window);

   return 0;
}

void WindowApp :: notify(EventBase* event)
{
   _eventBroadcaster->sendMessage(event);
}
