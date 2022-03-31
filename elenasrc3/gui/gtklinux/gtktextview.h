//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK TextView Control Header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKTEXTVIEW_H
#define GTKTEXTVIEW_H

#include "gtklinux/gtkcommon.h"
#include "guieditor.h"

namespace elena_lang
{
   // --- TextViewWindow ---
   class TextViewWindow : public Gtk::Table
   {
   public:
      class TextDrawingArea : public Gtk::DrawingArea
      {
      protected:
         Glib::RefPtr<Gdk::Window> _text_area;

         //Overrides:
         Gtk::SizeRequestMode get_request_mode_vfunc() const override;
         void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override;
         void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const override;
         void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override;
         void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override;
         void on_size_allocate(Gtk::Allocation& allocation) override;

         void onResize(int x, int y, int width, int height);

         void resizeDocument(int width, int height);

         void paint();

      public:s
         TextDrawingArea(TextViewWindow* view);
      };

   protected:
      TextDrawingArea _area;

   public:
      TextViewWindow(TextViewModelBase* model);
   };
}

#endif
