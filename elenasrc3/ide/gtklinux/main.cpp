//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Linux-GTK+ program entry
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtklinux/gtkcommon.h"
#include "factory.h"

using namespace elena_lang;

int main(int argc, char* argv[])
{
   Gtk::Main kit(argc, argv);

//   IDEModel      ideModel(10);
//   GUISettinngs  settings = { true };
   IDEFactory    factory/*(&ideModel, settings)*/;

   GUIApp* app = factory.createApp();
   GUIControlBase* ideWindow = factory.createMainWindow(app, /*&outputProcess*/nullptr, /*&vmConsoleProcess*/nullptr);

   int retVal = app->run(ideWindow, false, nullptr); // !! temporal

   delete app;

   return retVal;
}
