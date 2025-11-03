//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK Common Window Implementation
//                                              (C)2024-2025, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtkcommon.h"

using namespace elena_lang;

// --- GtkApp ---

void GtkApp :: on_my_startup()
{
   _appWindow->set_application(_app);

   _appWindow->show();
}

int GtkApp :: run(GUIControlBase* mainWindow, bool maximized, EventBase* startEvent)
{
   _appWindow = dynamic_cast<WindowWrapper*>(mainWindow)->getHandle();

   if (maximized)
      _appWindow->maximize();

   return _app->run(_argc, _argv);
}

void GtkApp :: notify(EventBase* event)
{
   _eventBroadcaster->sendMessage(event);
}
