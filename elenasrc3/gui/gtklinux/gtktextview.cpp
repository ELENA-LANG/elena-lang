//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK+ TextView Control Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtklinux/gtktextview.h"

using namespace elena_lang;

// --- TextDrawingArea ---

TextViewWindow::TextDrawingArea :: TextDrawingArea(TextViewWindow* view) :
   Glib::ObjectBase("textview"),
   Gtk::DrawingArea()
{
}

Gtk::SizeRequestMode TextViewWindow::TextDrawingArea :: get_request_mode_vfunc() const
{
   //Accept the default value supplied by the base class.
   return Gtk::DrawingArea::get_request_mode_vfunc();
}

void TextViewWindow::TextDrawingArea :: get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
{
   minimum_width = 50;
   natural_width = 0;
}

void TextViewWindow::TextDrawingArea :: get_preferred_height_for_width_vfunc(int /* width */,
   int& minimum_height, int& natural_height) const
{
   minimum_height = 50;
   natural_height = 0;
}

void TextViewWindow::TextDrawingArea :: get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
{
   minimum_height = 50;
   natural_height = 0;
}

void TextViewWindow::TextDrawingArea :: get_preferred_width_for_height_vfunc(int /* height */,
   int& minimum_width, int& natural_width) const
{
   minimum_width = 50;
   natural_width = 0;
}

void TextViewWindow::TextDrawingArea :: on_size_allocate(Gtk::Allocation& allocation)
{
//   /* Ensure h/v adj exist */
//   if(_view->setScrollerInfo(true, true, true)) {
////      gtk_adjustment_set_value(text_view->vadjustment, MAX(0, text_view->vadjustment->upper - text_view->vadjustment->page_size));
////      gtk_adjustment_set_value(text_view->hadjustment, MAX(0, text_view->hadjustment->upper - text_view->hadjustment->page_size));
//   }

  //Use the offered allocation for this container:
   set_allocation(allocation);

   if(_text_area)
   {
      _text_area->move_resize( allocation.get_x(), allocation.get_y(),
            allocation.get_width(), allocation.get_height() );

      onResize(allocation.get_x(), allocation.get_y(), allocation.get_width(), allocation.get_height());
   }
}

void TextViewWindow::TextDrawingArea :: onResize(int x, int y, int width, int height)
{
   resizeDocument(width, height);

   update(true);
}

void TextViewWindow::TextDrawingArea :: paint()
{
}

void TextViewWindow::TextDrawingArea :: resizeDocument(int width, int height)
{
   if (_model->isAssigned()) {
      Point     size;
      Rectangle client = getRectangle();
      auto style = _model->getStyle(STYLE_DEFAULT);

      int marginWidth = getLineNumberMargin();

      size.x = (client.width() - marginWidth) / style->avgCharWidth;
      size.y = client.height() / _model->getLineHeight();

      _model->resize(size);
   }
}

// --- TextViewWindow ---

TextViewWindow :: TextViewWindow(TextViewModelBase* model)
   : _area(this)
{
}
