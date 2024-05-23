//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK+ TextView Control Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtklinux/gtktextview.h"

using namespace elena_lang;

// --- ViewStyles ---

void ViewStyles::assign(pos_t count, StyleInfo* styles, int lineHeight, int marginWidth, FontFactory* factory)
{
   this->count = count;
   this->items = new Style[count];
   this->marginWidth = marginWidth;
   this->lineHeight = lineHeight;

   for (size_t i = 0; i < count; i++) {
      Font* font = factory->createFont(styles[i].faceName, styles[i].size,
         styles[i].bold, styles[i].italic);

      this->items[i] = { styles[i].foreground, styles[i].background, font };
   }
}

void ViewStyles::release()
{
   if (count > 0)
      delete[] items;

   count = 0;
}

// --- TextDrawingArea ---

TextViewWindow::TextDrawingArea :: TextDrawingArea(TextViewWindow* view/*, TextViewModelBase* model,
   ViewStyles* styles*/
) :
   Glib::ObjectBase("textview"),
   Gtk::DrawingArea()
{
   //_model = model;
   _needToResize = false;
   //_styles = styles;
}

Gtk::SizeRequestMode TextViewWindow::TextDrawingArea :: get_request_mode_vfunc() const
{
   //Accept the default value supplied by the base class.
   return Gtk::DrawingArea::get_request_mode_vfunc();
}

void TextViewWindow::TextDrawingArea :: get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
{
   minimum_width = 200;
   natural_width = 200;
}

void TextViewWindow::TextDrawingArea :: get_preferred_height_for_width_vfunc(int /* width */,
   int& minimum_height, int& natural_height) const
{
   minimum_height = 200;
   natural_height = 200;
}

void TextViewWindow::TextDrawingArea :: get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
{
   minimum_height = 200;
   natural_height = 200;
}

void TextViewWindow::TextDrawingArea :: get_preferred_width_for_height_vfunc(int /* height */,
   int& minimum_width, int& natural_width) const
{
   minimum_width = 200;
   natural_width = 200;
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

void TextViewWindow::TextDrawingArea :: on_map()
{
  //Call base class:
  Gtk::DrawingArea::on_map();
}

void TextViewWindow::TextDrawingArea :: on_unmap()
{
  //Call base class:
  Gtk::DrawingArea::on_unmap();
}

void TextViewWindow::TextDrawingArea :: on_realize()
{
  //Do not call base class Gtk::Widget::on_realize().
  //It's intended only for widgets that set_has_window(false).

   set_realized();

   if(!_text_area)
   {
      //Create the GdkWindow:

      GdkWindowAttr attributes;
      memset(&attributes, 0, sizeof(attributes));

      Gtk::Allocation allocation = get_allocation();

      //Set initial position and size of the Gdk::Window:
      attributes.x = allocation.get_x();
      attributes.y = allocation.get_y();
      attributes.width = allocation.get_width();
      attributes.height = allocation.get_height();

      attributes.event_mask = get_events () | Gdk::EXPOSURE_MASK
         | Gdk::BUTTON_PRESS_MASK
         | Gdk::BUTTON_RELEASE_MASK
         | Gdk::SCROLL_MASK
         | Gdk::KEY_PRESS_MASK
         | Gdk::POINTER_MOTION_MASK;
      attributes.window_type = GDK_WINDOW_CHILD;
      attributes.wclass = GDK_INPUT_OUTPUT;

      _text_area = Gdk::Window::create(get_parent_window(), &attributes,
               GDK_WA_X | GDK_WA_Y);
      set_window(_text_area);

      set_can_focus(TRUE);

      //set colors
//      override_background_color(Gdk::RGBA("red"));
//      override_color(Gdk::RGBA("blue"));

      //make the widget receive expose events
      _text_area->set_user_data(gobj());
   }
}

void TextViewWindow::TextDrawingArea :: on_unrealize()
{
   _text_area.reset();

   //Call base class:
   Gtk::DrawingArea::on_unrealize();
}

bool TextViewWindow::TextDrawingArea :: on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
   Canvas canvas(cr);

//   cairo_rectangle(canvas.cr, event->area.x, event->area.y, event->area.width, event->area.height);
//
//   cairo_clip (canvas.cr);

   paint(canvas, get_width(), get_height());

   return true;
}

void TextViewWindow::TextDrawingArea :: onResize(int x, int y, int width, int height)
{
   resizeDocument(width, height);

   update(true);
}

void TextViewWindow::TextDrawingArea :: paint(Canvas& canvas , int viewWidth, int viewHeight)
{
//   auto docView = _model->docView;
//
//   Point caret = docView->getCaret(false) - docView->getFrame();
//
//   Style* defaultStyle = _styles->getStyle(STYLE_DEFAULT);
//   Style* marginStyle = _styles->getStyle(STYLE_MARGIN);
//   int lineHeight = _styles->getLineHeight();
//   int marginWidth = _styles->getMarginWidth() + getLineNumberMargin();
//
//   if (!defaultStyle->valid) {
//      _styles->validate(canvas.layout);
//
//      lineHeight = _styles->getLineHeight();
//      marginWidth = _styles->getMarginWidth() + getLineNumberMargin();
//
//      _needToResize = true;
//   }
//   // if document size yet to be defined
//   if (_needToResize)
//      resizeDocument(viewWidth, viewHeight);
//
//   // draw background
//   canvas.fillRectangle(0, 0, viewWidth, viewHeight, defaultStyle);
//
//   // draw margin
//   canvas.fillRectangle(0, 0, marginWidth, viewHeight, marginStyle);
//
//   // draw text
//   int x = marginWidth;
//   int y = 1 - lineHeight;
//   int width = 0;
//
//   Style*                      style = defaultStyle;
//   char                        buffer[0x100];
//   StringTextWriter<char, 255> writer(buffer);
//   pos_t                       length = 0;
//
//   DocumentView::LexicalReader reader(docView);
//   reader.readFirst(writer, 255);
//   do {
//      style = _styles->getStyle(reader.style);
//      length = writer.position();
//
//      // set the text length
//      buffer[length] = 0;
//
//      if (reader.newLine) {
//         reader.newLine = false;
//
//         x = marginWidth;
//         y += lineHeight;
//      }
//      if (reader.bandStyle) {
//         canvas.fillRectangle(x, y, viewWidth, lineHeight + 1, marginStyle);
//
//         reader.bandStyle = false;
//      }
//
//      width = canvas.TextWidth(style, buffer);
//
//      /*else */canvas.drawText(x, y, buffer, style);
//
//      x += width;
//      writer.reset();
//   } while (reader.readNext(writer, 255));
}

int TextViewWindow::TextDrawingArea :: getLineNumberMargin()
{
//   if (_model->lineNumbersVisible) {
//      Style* marginStyle = _styles->getStyle(STYLE_MARGIN);
//
//      return marginStyle->avgCharWidth * 5;
//   }
   /*else*/ return 0;
}

void TextViewWindow::TextDrawingArea :: resizeDocument(int width, int height)
{
//   if (_model->isAssigned()) {
//      Point     size;
//      auto style = _styles->getStyle(STYLE_DEFAULT);
//
//      int marginWidth = getLineNumberMargin();
//
//      size.x = (width - marginWidth) / style->avgCharWidth;
//      size.y = height / _styles->getLineHeight();
//
//      _model->resize(size);
//
//      _needToResize = false;
//   }
}

void TextViewWindow::TextDrawingArea :: update(bool resized)
{
//   if (_model) {
//      _view->updateVScroller(resized);
//      if (_document->status.maxColChanged) {
//         _view->updateHScroller(true);
//	     _document->status.maxColChanged = false;
//      }
//      else _view->updateHScroller(resized);
//   }
   queue_draw();
}

// --- TextViewWindow ---

TextViewWindow :: TextViewWindow(/*TextViewModelBase* model, ViewStyles* styles*/)
   : _area(this/*, model, styles*/)
{
   attach(_area, 0, 1, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::EXPAND | Gtk::FILL, 0, 0);
}
