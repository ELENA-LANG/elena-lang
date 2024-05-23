//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK Common Header File
//                                             (C)2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKCOMMON_H
#define GTKCOMMON_H

#include <gtkmm.h>
#include "guicommon.h"

namespace elena_lang
{
   // --- Color ---
   struct Color
   {
      double red;
      double green;
      double blue;
      double alpha;

      bool operator == (Color& color)
      {
         if (red == color.red && green == color.green && blue == color.blue && alpha == color.alpha) {
            return true;
         }
         else return false;
      }

      bool operator != (Color& color)
      {
         if (red != color.red || green != color.green || blue != color.blue || alpha != color.alpha) {
            return true;
         }
         else return false;
      }

      void set(double red, double green, double blue)
      {
         this->red = red;
         this->green = green;
         this->blue = blue;
         this->alpha = 1.0;
      }

      void set(double red, double green, double blue, double alpha)
      {
         this->red = red;
         this->green = green;
         this->blue = blue;
         this->alpha = alpha;
      }

      Color()
      {
         set(0, 0, 0);
      }
      Color(double red, double green, double blue)
      {
         set(red, green, blue);
      }

      Color(double red, double green, double blue, double alpha)
      {
         set(red, green, blue, alpha);
      }
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

      WindowWrapper(Gtk::Window* window)
      {
         _window = window;
      }
      virtual ~WindowWrapper()
      {
         delete _window;
      }
   };

   // --- WindowApp ---
   class WindowApp : public GUIApp
   {
   public:
      void notify(EventBase* event) override;

      int run(GUIControlBase* mainWindow, bool maximized, EventBase* startEvent) override;
   };
}

#endif // GTKCOMMON_H
