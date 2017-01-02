//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK+ TextView Control Implementation File
//                                               (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtktextview.h"
//#include <gdk/gdkkeysyms.h>

using namespace _GUI_;

//static gint get_cursor_blink_timeout(TextViewWidget* textview)
//{
//   GtkSettings *settings = gtk_widget_get_settings(GTK_WIDGET(textview));
//   gint timeout;
//
//   g_object_get (settings, "gtk-cursor-blink-timeout", &timeout, NULL);
//
//   return timeout;
//}
//
//static gint get_cursor_time (TextViewWidget* textview)
//{
//   GtkSettings *settings = gtk_widget_get_settings (GTK_WIDGET(textview));
//   gint time;
//
//   g_object_get (settings, "gtk-cursor-blink-time", &time, NULL);
//
//   return time;
//}
//
//static gint onCursorBlink(gpointer data)
//{
//   TextViewWidget* widget = TEXT_VIEW(data);
//   TextView* textview = widget->view;
//   gint blink_timeout = get_cursor_blink_timeout(widget);
//
//   if (!gtk_widget_has_focus (GTK_WIDGET(widget))) {
//      textview->_resetBlink(false);
//
//      return FALSE;
//   }
//
//   if (textview->_getBlinkTime() > 1000 * blink_timeout && blink_timeout < G_MAXINT/1000) {
//      /* we've blinked enough without the user doing anything, stop blinking */
//      textview->showCaret();
//      textview->_resetBlinkTime();
//   }
//   else if (textview->isCaretVisible()) {
//      textview->hideCaret();
//      textview->_setBlinkTimeout(get_cursor_time(widget) * CURSOR_OFF_MULTIPLIER / CURSOR_DIVIDER);
//   }
//   else {
//      textview->showCaret();
//      textview->_incBlinkTime(get_cursor_time(widget));
//
//      textview->_setBlinkTimeout(get_cursor_time(widget) * CURSOR_ON_MULTIPLIER / CURSOR_DIVIDER);
//   }
//
//   /* Remove ourselves */
//   return FALSE;
//}

// --- ViewStyles ---

void ViewStyles :: release()
{
   if (_count > 0)
      delete[] _styles;

   _count = 0;
}

void ViewStyles :: assign(int count, StyleInfo* styles, int lineHeight, int marginWidth)
{
   _count = count;
   _styles = new Style[count];
   _lineHeight = lineHeight;
   _marginWidth = marginWidth;

   for (int i = 0 ; i < count ; i++) {
      Font* font = Font::createFont(styles[i].faceName, styles[i].size, styles[i].bold, styles[i].italic);

      _styles[i] = Style(styles[i].foreground, styles[i].background, font);
   }
}

void ViewStyles :: validate(Glib::RefPtr<Pango::Layout> layout)
{
   for (int i = 0 ; i < _count ; i++) {
      _styles[i].validate(layout);
      if (_lineHeight < _styles[i].lineHeight) {
         _lineHeight = _styles[i].lineHeight;
      }
   }
}

// --- TextView Widget functions

//void TextView :: _dispose(GObject *object)
//{
//   G_OBJECT_CLASS (text_view_parent_class)->dispose(object);
//}

void TextView :: on_vscroll()
{
   _area.onVScroll(_vadjustment->get_value());
}

void TextView :: on_hscroll()
{
   _area.onHScroll(_hadjustment->get_value());
}

////gint TextView :: _motion_event(GtkWidget* widget, GdkEventMotion* event)
////{
////   TextViewWidget* text_view = TEXT_VIEW(widget);
////
////   return text_view->view->_onMouseMove(Point(event->x, event->y));
////}
////
////gint TextView :: _focus_in_event(GtkWidget* widget, GdkEventFocus* event)
////{
////   TextViewWidget* text_view = TEXT_VIEW(widget);
////
////   if (TEXT_VIEW(widget)->view->_document) {
////      gtk_widget_queue_draw (widget);
////
////      text_view->view->_resetBlinkTime();
////      text_view->view->_resetBlink(true);
////   }
////   return FALSE;
////}
////
////gint TextView :: _focus_out_event(GtkWidget* widget, GdkEventFocus* event)
////{
////   TextViewWidget* text_view = TEXT_VIEW(widget);
////
////   gtk_widget_queue_draw (widget);
////
////   text_view->view->_resetBlink(false);
////
////   return FALSE;
////}

// --- TextView::TextDrawingArea ---

TextView::TextDrawingArea :: TextDrawingArea(TextView* view) :
   Glib::ObjectBase("textview"),
   Gtk::DrawingArea()
{
   // gtkmm__CustomObject_textview_CLASS initialization
//   GtkBindingSet* binding_set = gtk_binding_set_by_class(cls);

   // Events
/*
   add_events(Gdk::KEY_PRESS_MASK);
   signal_key_press_event().connect(sigc::mem_fun(*this,
         &TextView::on_key_pressed));
*/
//   _initCallbacks();
//   _initResizeCallback();

   _view = view;
   _document = NULL;
   _needToResize = false;

   _tabUsing = false;
   _tabSize = 4;

//   _blinkTime = 0;
   _caretVisible = true;       // !! temporal
   _lineNumbersVisible = true; // !! temporal
   _mouseCaptured = false;
   _highlight = false;
   _caret_x = 0;

//   _cached = false;
}

// --- TextView ---

TextView :: TextView()
   : _area(this)
{
   _vadjustment = _vscrollbar.get_adjustment();
   _hadjustment = _hscrollbar.get_adjustment();

   attach(_area, 0, 1, 0, 1, Gtk::EXPAND | Gtk::FILL, Gtk::EXPAND | Gtk::FILL, 0, 0);
   attach(_hscrollbar, 0, 1, 1, 2, Gtk::EXPAND | Gtk::FILL, Gtk::FILL, 0, 0);
   attach(_vscrollbar, 1, 2, 0, 1, Gtk::FILL, Gtk::EXPAND | Gtk::FILL, 0, 0);

//   set_policy(Gtk::POLICY_ALWAYS, Gtk::POLICY_ALWAYS);

   _hadjustment->signal_value_changed().connect(
      sigc::mem_fun(*this, &TextView::on_hscroll));

   _vadjustment->signal_value_changed().connect(
      sigc::mem_fun(*this, &TextView::on_vscroll));
}

//TextView :: ~TextView()
//{
//   // to remove cursor blink timeout
//   _resetBlink(false);
//}

bool TextView::TextDrawingArea :: on_key_press_event(GdkEventKey* event)
{
   if (!_document || _document->status.readOnly)
      return Gtk::DrawingArea::on_key_press_event(event);

   if (event->type == GDK_KEY_PRESS) {
      bool shift = _ELENA_::test(event->state, GDK_SHIFT_MASK);
      if (_ELENA_::test(event->state, GDK_CONTROL_MASK)) {
         switch (event->keyval) {
            case GDK_KEY_Right:
               _document->moveRightToken(shift);
               break;
            case GDK_KEY_Left:
               _document->moveLeftToken(shift);
               break;
            case GDK_KEY_Down:
               _document->moveFrameDown();
               break;
            case GDK_KEY_Up:
               _document->moveFrameUp();
               break;
            case GDK_KEY_Home:
               _document->moveFirst(shift);
               break;
            case GDK_KEY_End:
               _document->moveLast(shift);
               break;
            default:
               return Gtk::DrawingArea::on_key_press_event(event);
         }
      }
      else {
         switch (event->keyval) {
            case GDK_KEY_Right:
               _document->moveRight(shift);
               break;
            case GDK_KEY_Left:
               _document->moveLeft(shift);
               break;
            case GDK_KEY_Down:
               _document->moveDown(shift);
               break;
            case GDK_KEY_Up:
               _document->moveUp(shift);
               break;
            case GDK_KEY_Home:
               _document->moveHome(shift);
               break;
            case GDK_KEY_End:
               _document->moveEnd(shift);
               break;
            case GDK_KEY_Page_Up:
               _document->movePageUp(shift);
               break;
            case GDK_KEY_Page_Down:
               _document->movePageDown(shift);
               break;
            case GDK_KEY_Return:
               _document->insertNewLine();
               break;
            case GDK_KEY_BackSpace:
               _document->eraseChar(true);
               break;
            case GDK_KEY_Tab:
               if (_ELENA_::test(event->state, GDK_CONTROL_MASK)) {
      //            g_signal_emit (GTK_OBJECT(_handle), signals[CTRLTAB_PRESSED],
      //                           0, _ELENA_::test(event->state, GDK_SHIFT_MASK));
               }
               else indent();
               break;
            case GDK_KEY_Delete:
               _document->eraseChar(false);
               break;
            case GDK_KEY_Insert:
               _document->setOverwriteMode(!_document->status.overwriteMode);
               break;
            default:
            {
               guint32 unichar = gdk_keyval_to_unicode(event->keyval);
               if (unichar >= 0x20) {
                  char utf8string[10];
                  int count = g_unichar_to_utf8(unichar, utf8string);

                  if (count > 1) {
                     _document->insertLine(utf8string, count);
                  }
                  else _document->insertChar(utf8string[0]);
               }
               else return Gtk::DrawingArea::on_key_press_event(event);
            }
         }
      }
      onEditorChange();

      return true;
   }
   else return Gtk::DrawingArea::on_key_press_event(event);
}

bool TextView::TextDrawingArea :: on_button_press_event(GdkEventButton* event)
{
   if (event->type == GDK_BUTTON_PRESS/* && event->window == text_view->text_area*/) {
      grab_focus();

      if (_document) {
         bool kbShift = event->state & GDK_SHIFT_MASK;
         Point point(event->x, event->y);

//      _resetBlink(true);

         int col = 0, row = 0;
         bool margin = false;
         if(mouseToScreen(point, col, row, margin))
            _document->moveToFrame(col, row, kbShift);
   //   if (margin) {
   //      notify(IDE_EDITOR_MARGINCLICKED);
   //   }
         _captureMouse();

   //   refresh(true);
         onEditorChange();
      }
      return true;
   }
   else return false;
}

bool TextView::TextDrawingArea :: on_button_release_event (GdkEventButton* event)
{
   //_releaseMouse();

   return true;
}

bool TextView::TextDrawingArea :: on_scroll_event(GdkEventScroll* scroll_event)
{
   if (_document) {
      int offset = (scroll_event->direction == GDK_SCROLL_UP) ? -1 : 1;

      if (_ELENA_::test(scroll_event->state, GDK_CONTROL_MASK)) {
         offset *= _document->getSize().y;
      }
      _document->vscroll(offset);

      onEditorChange();
   }
}

void TextView::TextDrawingArea :: onResize(int x, int y, int width, int height)
{
   resizeDocument(width, height);
//
//   _zbuffer.release();

   update(true);
}

void TextView::TextDrawingArea :: onEditorChange()
{
//   if (_document->status.isViewChanged()) {
//      _cached = false;
//
//      refresh();
//   }

   update(false);

   _view->_textview_changed.emit();
}

void TextView::TextDrawingArea :: onHScroll(int newPosition)
{
   Point frame = _document->getFrame();

   _document->hscroll(newPosition - frame.x);

   queue_draw();
}

void TextView::TextDrawingArea :: onVScroll(int newPosition)
{
   Point frame = _document->getFrame();

   _document->vscroll(newPosition - frame.y);

   queue_draw();
}

//int TextView :: _onMouseMove(Point point)
//{
//   if (_isMouseCaptured() && _document != NULL) {
//      int col = 0, row = 0;
//      bool margin = false;
//      if(mouseToScreen(point, col, row, margin))
//         _document->moveToFrame(col, row, true);
//
//      onEditorChange();
//
//      return TRUE;
//   }
//   else return FALSE;
//}
//

Gtk::SizeRequestMode TextView::TextDrawingArea ::get_request_mode_vfunc() const
{
   //Accept the default value supplied by the base class.
   return Gtk::DrawingArea::get_request_mode_vfunc();
}

void TextView::TextDrawingArea ::get_preferred_width_vfunc(int& minimum_width, int& natural_width) const
{
   minimum_width = 50;
   natural_width = 0;
}

void TextView::TextDrawingArea :: get_preferred_height_for_width_vfunc(int /* width */,
   int& minimum_height, int& natural_height) const
{
   minimum_height = 50;
   natural_height = 0;
}

void TextView::TextDrawingArea :: get_preferred_height_vfunc(int& minimum_height, int& natural_height) const
{
   minimum_height = 50;
   natural_height = 0;
}

void TextView::TextDrawingArea :: get_preferred_width_for_height_vfunc(int /* height */,
   int& minimum_width, int& natural_width) const
{
   minimum_width = 50;
   natural_width = 0;
}

void TextView::TextDrawingArea :: on_size_allocate(Gtk::Allocation& allocation)
{
//   if (gtk_widget_get_realized (widget))
//   {
//      gint x = widget->allocation.x;
//      gint y = widget->allocation.y;
//      gint width = widget->allocation.width;
//      gint height = widget->allocation.height;
//
//      gdk_window_move_resize (widget->window, x, y, width, height);
//      gdk_window_move_resize (text_view->text_area, 0, 0, width, height);
//
//   }

   /* Ensure h/v adj exist */
   if(_view->setScrollerInfo(true, true, true)) {
//      gtk_adjustment_set_value(text_view->vadjustment, MAX(0, text_view->vadjustment->upper - text_view->vadjustment->page_size));
//      gtk_adjustment_set_value(text_view->hadjustment, MAX(0, text_view->hadjustment->upper - text_view->hadjustment->page_size));
   }

//   gtk_adjustment_changed(text_view->hadjustment);
//   gtk_adjustment_changed(text_view->vadjustment);

  //Use the offered allocation for this container:
   set_allocation(allocation);

   if(_text_area)
   {
      _text_area->move_resize( allocation.get_x(), allocation.get_y(),
            allocation.get_width(), allocation.get_height() );

      onResize(allocation.get_x(), allocation.get_y(), allocation.get_width(), allocation.get_height());
   }
}

void TextView::TextDrawingArea :: on_map()
{
  //Call base class:
  Gtk::DrawingArea::on_map();
}

void TextView::TextDrawingArea :: on_unmap()
{
  //Call base class:
  Gtk::DrawingArea::on_unmap();
}

void TextView::TextDrawingArea :: on_realize()
{
//   TextViewWidget* text_view = TEXT_VIEW(widget);
//   GdkWindowAttr   attributes;
//   gint            attributes_mask;
//
//   GdkCursor* cursor = gdk_cursor_new_for_display (gdk_drawable_get_display (widget->window),
//					                                     GDK_XTERM);
//   gdk_window_set_cursor(widget->window, cursor);
//   gdk_cursor_unref(cursor);
//
//   text_view->text_area = gdk_window_new (widget->window, &attributes, attributes_mask);
//
//   gdk_window_show(text_view->text_area);

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

//   GTK_WIDGET_UNSET_FLAGS(widget, GTK_DOUBLE_BUFFERED);

      set_can_focus(TRUE);

      //set colors
//      override_background_color(Gdk::RGBA("red"));
//      override_color(Gdk::RGBA("blue"));

      //make the widget receive expose events
      _text_area->set_user_data(gobj());
   }
}

void TextView::TextDrawingArea :: on_unrealize()
{
//   if (text_view->text_area) {
//      gdk_window_set_user_data (text_view->text_area, NULL);
//      gdk_window_destroy (text_view->text_area);
//   }
   _text_area.reset();

   //Call base class:
   Gtk::DrawingArea::on_unrealize();
}
//
//void TextView :: _finalize(GObject *object)
//{
//   TextViewWidget* text_view = TEXT_VIEW(object);
//
//   text_view->view->hideCaret();
//
//   G_OBJECT_CLASS (text_view_parent_class)->finalize(object);
//}

bool TextView::TextDrawingArea :: on_draw(const Cairo::RefPtr<Cairo::Context>& cr)
{
   Canvas canvas(cr);

//   cairo_rectangle(canvas.cr, event->area.x, event->area.y, event->area.width, event->area.height);
//
//   cairo_clip (canvas.cr);

   paint(canvas, get_width(), get_height());

   return true;
}

void TextView::TextDrawingArea :: paint(Canvas& /*extCanvas*/canvas , int viewWidth, int viewHeight)
{
//   if (_zbuffer.isReleased()) {
//      _cached = false;
//
//      _zbuffer.init(extCanvas, viewWidth, viewWidth);
//   }

   Style defaultStyle = _styles[STYLE_DEFAULT];
   Style marginStyle = _styles[STYLE_MARGIN];
   Style style = defaultStyle;

   if (!defaultStyle.valid) {
      _styles.validate(/*extCanvas*/canvas.layout);

      defaultStyle = _styles[STYLE_DEFAULT];
      marginStyle = _styles[STYLE_MARGIN];

      _needToResize = true;
   }
   // if document size yet to be defined
   if (_needToResize) {
      resizeDocument(viewWidth, viewHeight);
      _view->updateVScroller(true);
      _view->updateHScroller(true);
   }

   size_t lineHeight = _styles.getLineHeight();
   size_t marginWidth = _styles.getMarginWidth();
   if (_lineNumbersVisible) {
      marginWidth += getLineNumberMargin();
   }

//   if (!_cached) {
//      Canvas canvas(_zbuffer);

      // Draw background
      canvas.fillRectangle(0, 0, viewWidth, viewHeight, defaultStyle);

      // Draw margin
      canvas.fillRectangle(0, 0, marginWidth, viewHeight, marginStyle);

      // Draw text
      int x = marginWidth;
      int y = 1 - lineHeight;
      int width = 0;

      char buffer[0x100];
      int   length = 0;

      _ELENA_::String<char, 6> lineNumber;

      _ELENA_::LiteralWriter<char> writer(buffer, 0xFF);
      Document::Reader reader(_document);

      reader.readFirst(writer, 0xFF);
      do {
         style = _styles[reader.style];
         length = writer.Position();

         // set the text length
         buffer[length] = 0;

         if (reader.newLine) {
            reader.newLine = false;

            x = marginWidth;
            y += lineHeight;

            if (_lineNumbersVisible) {
               lineNumber.copyInt(reader.row + 1);
               canvas.drawText(
                     x - marginStyle.avgCharWidth * _ELENA_::getlength(lineNumber) - 4,
                     y,
                     lineNumber,
                     marginStyle);
            }
            // !! HOTFIX: allow to see breakpoint ellipse on margin if STYLE_TRACELINe set for this line

            if (reader.marker) {
               //canvas.drawEllipse(Rectangle(3, y + 2, 12, 12), style); // !! temporal

               reader.marker = false;
            }
            //if (info.style == STYLE_BREAKPOINT ||
            //   (info.bandLine && _currentDoc->checkMarker(info.bookmark.getRow(), STYLE_BREAKPOINT)))
            //{
            //   canvas.drawEllipse(Rectangle(3, y + 2, 12, 12), style);
            //}
         }

         //width = (style.avgCharWidth) * length;
         width = canvas.TextWidth(&style, buffer);

         if (reader.bandStyle) {
            canvas.fillRectangle(x, y, viewWidth, lineHeight + 1, marginStyle);

            reader.bandStyle = false;
         }
         else if (reader.style != STYLE_DEFAULT && defaultStyle.background != style.background) {
            canvas.fillRectangle(x, y, width, lineHeight + 1, style);
         }

         //if (defaultStyle._background != style._background) {
         if (length==0 && reader.style==STYLE_SELECTION) {
            canvas.fillRectangle(x, y, defaultStyle.avgCharWidth, lineHeight + 1, style);
         }
         else canvas.drawText(x, y, buffer, style);
         //}
         //else canvas.drawTextClippedTransporent(Rectangle(x, y, width, lineHeight), x, y, info.line, info.length, style);

         x += width;
         writer.reset();
      }
      while (reader.readNext(writer, 0xFF));

//      _cached = true;
   //}

//   // copy zbuffer
//   extCanvas.copy(_zbuffer, 0, 0);

   // Draw cursor
   Point caret = _document->getCaret(false) - _document->getFrame();
   if (_caretVisible && caret.x >= 0 && caret.y >= 0) {
      _caret_x = 0;

      if (_document->status.caretChanged)
         _caret_x = 0;

      writer.reset();

      reader.initCurrentLine();
      reader.readCurrentLine(writer, 0xFF);
      do {
         Style style = _styles[reader.style];
         // set the text length
         buffer[writer.Position()] = 0;

         _caret_x += canvas.TextWidth(&style, buffer);

         writer.reset();
      } while (reader.readCurrentLine(writer, 0xFF));

      _document->status.caretChanged = false;

      if (!_document->status.overwriteMode) {
         /*extC*/canvas.drawCursor(marginWidth + _caret_x, lineHeight * caret.y + 1, style);
      }
      else /*extC*/canvas.drawOverwriteCursor(marginWidth + _caret_x, lineHeight * caret.y + 1, style);
   }
}

////Point TextView :: getSize()
////{
////   Point size = Point(_width, _height);
////
////   return size;
////}

void TextView :: setStyles(int count, StyleInfo* styles, int lineHeight, int marginWidth)
{
   _area._styles.release();
   _area._styles.assign(count, styles, lineHeight, marginWidth);

//   _cached = false;
}

void TextView ::TextDrawingArea :: resizeDocument(int width, int height)
{
   _needToResize = true;
   if (_styles.Count() > 0 && _styles[0].valid && _document != NULL) {
      Point     size;

      int marginWidth = /*getLineNumberMargin()*/0;
      size.x = (width - marginWidth) / _styles[0].avgCharWidth;
      size.y = height / _styles.getLineHeight();

      _document->resize(size);

      _needToResize = false;
      //_cached = false;
   }
}

void TextView ::TextDrawingArea :: update(bool resized)
{
   if (_document) {
      _view->updateVScroller(resized);
      if (_document->status.maxColChanged) {
         _view->updateHScroller(true);
	     _document->status.maxColChanged = false;
      }
      else _view->updateHScroller(resized);
   }
   queue_draw();
}

void TextView :: setDocument(Document* document)
{
   _area._document = document;
   _area._document->setHighlightMode(_area._highlight);

   _area._needToResize = true;
//   _cached = false;
//   refresh();
}

//void TextView :: _setBlinkTimeout(int timeout)
//{
//   _blinkTimeout = gdk_threads_add_timeout(timeout, onCursorBlink, _handle);
//}
//
//void TextView :: _resetBlink(bool hasFocus)
//{
//   if (hasFocus) {
//      if (_blinkTimeout == 0) {
//         _setBlinkTimeout(get_cursor_time(TEXT_VIEW(_handle)) * CURSOR_ON_MULTIPLIER / CURSOR_DIVIDER);
//      }
//   }
//   else {
//      if (_blinkTimeout != 0) {
//         g_source_remove(_blinkTimeout);
//         _blinkTimeout = 0;
//      }
//      _caretVisible = false;
//   }
//}
//
//void TextView :: _pendBlink(bool hasFocus)
//{
//   if (hasFocus) {
//      if (_blinkTimeout != 0) {
//         g_source_remove (_blinkTimeout);
//         _blinkTimeout = 0;
//      }
//
//      _setBlinkTimeout(get_cursor_time(TEXT_VIEW(_handle)) * CURSOR_PEND_MULTIPLIER / CURSOR_DIVIDER);
//
//      _caretVisible = true;
//   }
//}
//
//void TextView :: showCaret()
//{
//   _caretVisible = true;
//
//   refresh();
//}
//
//void TextView :: hideCaret()
//{
//   _caretVisible = false;
//   _resetBlink(false);
//
//   refresh();
//}

bool TextView :: setScrollerInfo(bool resized, bool vertical, bool horizontal)
{
   if (_area._needToResize)
      return false;

   bool changed = false;

   Point size = _area._document->getSize();
   Point frame = _area._document->getFrame();

   if (horizontal) {
      changed |= (_hadjustment->get_value() != frame.x);

      if (resized || changed) {
         _hadjustment->set_page_size(size.x);
         _hadjustment->set_page_increment(size.x * 0.9);
         _hadjustment->set_step_increment(1);
         _hadjustment->set_lower(0);
         _hadjustment->set_upper(MAX(_area._document->getMaxColumn(), size.x));
      }
      if (changed)
         _hadjustment->set_value(frame.x);
   }

   if (vertical) {
      changed |= (_vadjustment->get_value() != frame.y);

      if (resized || changed) {
         _vadjustment->set_page_size(size.y);
         _vadjustment->set_page_increment(size.y * 0.9);
         _vadjustment->set_step_increment(1);
         _vadjustment->set_lower(0);
         _vadjustment->set_upper(size.y + _area._document->getRowCount());
      }
      if (changed)
         _vadjustment->set_value(frame.y);
   }
   return changed;
}

void TextView :: updateHScroller(bool resized)
{
   if(setScrollerInfo(resized, false, true)) {
      _hadjustment->value_changed();
   }
   if (resized)
      _hadjustment->changed();
}

void TextView :: updateVScroller(bool resized)
{
   if(setScrollerInfo(resized, true, false)) {
      _vadjustment->value_changed();
   }
   if (resized)
      _vadjustment->changed();
}

int TextView::TextDrawingArea :: getLineNumberMargin()
{
   if (_lineNumbersVisible) {
      Style marginStyle = _styles[STYLE_MARGIN];

      return marginStyle.avgCharWidth * 5;
   }
   else return 0;
}

void TextView::TextDrawingArea :: indent()
{
   if (_tabUsing) {
      _document->tabbing('\t', 1, true);
   }
   else {
      if (!_document->hasSelection()) {
         size_t shift = _ELENA_::calcTabShift(_document->getCaret().x, _tabSize);
         _document->insertChar(' ', shift);
      }
      else _document->tabbing(' ', _tabSize, true);
   }
   queue_draw();
}

void TextView::TextDrawingArea :: outdent()
{
   if (_document->hasSelection()) {
      if (_tabUsing) {
         _document->tabbing('\t', 1, false);
      }
      else _document->tabbing(' ', _tabSize, false);
   }
   queue_draw();
}

bool TextView::TextDrawingArea :: mouseToScreen(Point point, int& col, int& row, bool& margin)
{
//   //Rectangle rect = getRectangle();
   Style defaultStyle = _styles[STYLE_DEFAULT];
   if (defaultStyle.valid) {
      int marginWidth = _styles.getMarginWidth() + getLineNumberMargin();
      int offset = defaultStyle.avgCharWidth / 2;

      col = (point.x/* - rect.topLeft.x*/ - marginWidth + offset) / defaultStyle.avgCharWidth;
      row = (point.y/* - rect.topLeft.y*/) / (_styles.getLineHeight());
      margin = (point.x/* - rect.topLeft.x*/ < marginWidth);

      return true;
   }
   else return false;
}

void TextView::TextDrawingArea :: _captureMouse()
{
   _mouseCaptured = true;
}

void TextView::TextDrawingArea :: _releaseMouse()
{
   _mouseCaptured = false;
}

//void TextView :: applySettings(int tabSize, bool tabUsing, bool lineNumberVisible, bool highlight)
//{
//   _highlight = highlight;
//
//   if (_document) {
//      _document->setHighlightMode(_highlight);
//
//      refreshView();
//   }
//}
//

void TextView :: applySettings(int tabSize, bool tabUsing, bool lineNumberVisible, bool highlight)
{
   _area._needToResize = (_area._lineNumbersVisible != lineNumberVisible);
   //bool tabChanged = (_area._tabSize != tabSize);
   //_cached &= !(_area._needToResize || _area._tabSize != tabSize || _highlight != highlight);

   _area._tabUsing = tabUsing;
   _area._tabSize = tabSize;
   _area._lineNumbersVisible = lineNumberVisible;
   _area._highlight = highlight;

   if (_area._document) {
      _area._document->setHighlightMode(_area._highlight);

      refreshView();
   }
}

type_textview_changed TextView :: textview_changed()
{
   return _textview_changed;
}

