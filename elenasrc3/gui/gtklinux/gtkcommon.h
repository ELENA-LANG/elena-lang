//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI common classes header File
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKCOMMON_H
#define GTKCOMMON_H

#include "guicommon.h"
#include <gtkmm.h>

namespace elena_lang
{
   // --- BroadcasterBase ---
   class BroadcasterBase
   {
   public:
      virtual void sendMessage(EventBase* event) = 0;
   };

   // --- WindowBase ---
   class WindowWrapper : public GUIControlBase
   {
      Gtk::Window* _window;

   public:
      Gtk::Window* getHandle() { return _window; }

      bool checkHandle(void* param) const
      {
         return (void*)_window == param;
      }

      Rectangle getRectangle() override { return {}; }
      void setRectangle(Rectangle rec) override {}

      void show() override
      {
         _window->show();
      }

      void hide() override
      {
         _window->hide();
      }

      virtual bool visible()
      {
         return _window->is_visible();
      }

      void setFocus() override {}

      void refresh() override {}

      void invalidate() override {}

      WindowWrapper(Gtk::Window* window)
      {
         _window = window;
      }
      virtual ~WindowWrapper()
      {
         delete _window;
      }
   };

   // --- GtkApp ---
   class GtkApp : public GUIApp
   {
      int                            _argc;
      char**                         _argv;

      Gtk::Window*                   _appWindow;

      BroadcasterBase*               _eventBroadcaster;
      Glib::RefPtr<Gtk::Application> _app;

      void on_my_startup();

   public:
      int run(GUIControlBase* mainWindow, bool maximized, EventBase* startEvent) override;

      void quit()
      {
         _app->quit();

         //_appWindow->set_visible(false);
      }

      void notify(EventBase* event) override;

      GtkApp(int argc, char** argv, BroadcasterBase* eventBroadcaster)
      {
         _argc = argc;
         _argv = argv;

         _eventBroadcaster = eventBroadcaster;

         _app = Gtk::Application::create();
         _appWindow = nullptr;

         _app->signal_startup().connect(sigc::mem_fun(*this,
              &GtkApp::on_my_startup));
      }
   };
}

#endif
