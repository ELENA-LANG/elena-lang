//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK TextView Control Header File
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKTEXTVIEW_H
#define GTKTEXTVIEW_H

#include "gtklinux/gtkcommon.h"
#include "gtklinux/gtkgraphic.h"
#include "guieditor.h"

namespace elena_lang
{
   // --- StyleInfo ---
   struct StyleInfo
   {
      Color       foreground;
      Color       background;
      const char* faceName;
      int         size;
      bool        bold;
      bool        italic;
   };

   // --- ViewStyles ---
   struct ViewStyles
   {
      Style* items;
      pos_t  count;

      int    marginWidth;
      int    lineHeight;

      void assign(pos_t count, StyleInfo* styles, int lineHeight, int marginWidth, FontFactory* factory);
      void release();

      Style* getStyle(pos_t index) const
      {
         return &items[index];
      }

      int getMarginWidth() const
      {
         return marginWidth;
      }

      int getLineHeight() const
      {
         return lineHeight;
      }

      void validate(Glib::RefPtr<Pango::Layout> layout)
      {
         for (pos_t i = 0; i < count; i++) {
            items[i].validate(layout);

            if (items[i].valid && lineHeight < items[i].lineHeight)
               lineHeight = items[i].lineHeight;
         }
      }

      ViewStyles()
      {
         lineHeight = marginWidth = 0;
         items = nullptr;
         count = 0;
      }
   };

   // --- TextViewWindow ---
   class TextViewWindow : public Gtk::/*Table*/ScrolledWindow
   {
   public:
      class TextDrawingArea : public Gtk::DrawingArea
      {
      protected:
         TextViewWindow*            _view;

         Glib::RefPtr<Gtk::Window>  _text_area;

         TextViewModelBase*         _model;
         bool                       _needToResize;
         bool                       _caretVisible;
         bool                       _caretChanged;
         int                        _caret_x;
         ViewStyles*                _styles;
         TextViewControllerBase*    _controller;

         //Overrides:
//         Gtk::SizeRequestMode get_request_mode_vfunc() const override;
//         void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const override;
//         void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const override;
//         void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const override;
//         void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const override;
//         void on_size_allocate(Gtk::Allocation& allocation) override;
//         void on_map() override;
//         void on_unmap() override;
//         void on_realize() override;
//         void on_unrealize() override;
         void on_draw(const Cairo::RefPtr<Cairo::Context>& cr, int width, int height);
//
//         bool on_key_press_event(GdkEventKey* event) override;
//         bool on_button_press_event(GdkEventButton* event) override;
//         bool on_button_release_event (GdkEventButton* event) override;
//         bool on_scroll_event (GdkEventScroll* scroll_event) override;

         void onResize(int x, int y, int width, int height);

         bool mouseToScreen(Point point, int& col, int& row, bool& margin);

         int getLineNumberMargin();
         void resizeDocument(int width, int height);
         void update(bool resized);

         void paint(Canvas& canvas, int viewWidth, int viewHeight);

      public:
         void onDocumentUpdate(DocumentChangeStatus& changeStatus);

         TextDrawingArea(TextViewWindow* view, TextViewModelBase* model,
            TextViewControllerBase* controller, ViewStyles* styles);
      };

   protected:
      TextDrawingArea       _area;

//      void on_grab_focus() override
//      {
//         _area.grab_focus();
//      }

   public:
      void onDocumentUpdate(DocumentChangeStatus& changeStatus);

      void updateVScroller(bool resized);

      TextViewWindow(TextViewModelBase* model, TextViewControllerBase* controller,
         ViewStyles* styles);
   };
}

#endif
