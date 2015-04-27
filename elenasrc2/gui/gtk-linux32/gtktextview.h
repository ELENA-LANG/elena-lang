//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK TextView Control Header File
//                                               (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef gtktextviewH
#define gtktextviewH

#include "gtkcommon.h"
#include "gtkgraphic.h"
#include "document.h"

namespace _GUI_
{
// --- StyleInfo ---

struct StyleInfo
{
   Colour      foreground;
   Colour      background;
   const char* faceName;
};

// --- ViewStyle ---

class ViewStyles
{
   Style* _styles;
   int    _count;

   int   _lineHeight;
   int   _marginWidth;

public:
   int Count() const { return _count; }

   Style operator[](unsigned int index)
   {
      return _styles[index];
   }

   void assign(int count, StyleInfo* styles, int lineHeight, int marginWidth);

   int getLineHeight() const { return _lineHeight; }
   int getMarginWidth() const { return _marginWidth; }

   void validate(Glib::RefPtr<Pango::Layout> layout);

   void release();

   ViewStyles() { _marginWidth = _lineHeight = _count = 0; }
   ~ViewStyles() { release(); }
};

// --- TextView ---

class TextView : public Gtk::Table
{
protected:
//public:
////   //// widget functions
////   //static gboolean _expose(GtkWidget* textview, GdkEventExpose* event);
////   //static void _dispose(GObject* object);
////   ////static void text_view_grab_notify(GtkWidget* widget, gboolean was_grabbed);
////   //static void _size_request(GtkWidget* widget, GtkRequisition* requisition);
////   ////static gint text_view_event(GtkWidget* widget, GdkEvent* event);
////   //static gint _key_release_event(GtkWidget* widget, GdkEventKey* event);
////   //static gint _button_press_event(GtkWidget* widget, GdkEventButton* event);
////   //static gint _button_release_event(GtkWidget* widget, GdkEventButton* event);
////   //static gint _focus_in_event(GtkWidget* widget, GdkEventFocus* event);
////   //static gint _focus_out_event(GtkWidget* widget, GdkEventFocus* event);
////   //static gint _motion_event(GtkWidget* widget, GdkEventMotion* event);
////   ////static gboolean text_view_focus(GtkWidget* widget, GtkDirectionType direction);
////
////   //static void _move_cursor(_TextViewWidget *text_view, int keyAction, int isSelected);
////
////   //virtual GtkWidget* getHandle() const { return _scrollerWindow; } // scroller window should be considered as a control handle
////   //GtkWidget* getWidgetHandle() const { return _handle; }
////
////   //void _resetImContext();
////
////   //int _getBlinkTime() const { return _blinkTime; }
////   //void _incBlinkTime(int time)
////   //{
////   //   _blinkTime += time;
////   //}
////   //void _resetBlinkTime()
////   //{
////   //   _blinkTime = 0;
////   //}
////   //void _setBlinkTimeout(int timeout);
////
////   //bool isCaretVisible() const { return _caretVisible; }
////
////   //void _resetBlink(bool hasFocus);
////   //void _pendBlink(bool hasFocus);
////   //void showCaret();
////   //void hideCaret();
////   //virtual void onKeyDown(_KeyAction key, bool selection);
////   //int _onMouseMove(Point point);
////
//   // object methods
//
////   //virtual void refreshView()
////   //{
////   //   onEditorChange();
////   //}
////
////   //void applySettings(int tabSize, bool tabUsing, bool lineNumberVisible, bool highlight);

   class TextDrawingArea : public Gtk::DrawingArea
   {
   protected:
      TextView*                 _view;

      Glib::RefPtr<Gdk::Window> _text_area;

//   //bool         _cached;
//   //DoubleBuffer _zbuffer;

      bool        _caretVisible;

      //Overrides:
      virtual Gtk::SizeRequestMode get_request_mode_vfunc() const;
      virtual void get_preferred_width_vfunc(int& minimum_width, int& natural_width) const;
      virtual void get_preferred_height_for_width_vfunc(int width, int& minimum_height, int& natural_height) const;
      virtual void get_preferred_height_vfunc(int& minimum_height, int& natural_height) const;
      virtual void get_preferred_width_for_height_vfunc(int height, int& minimum_width, int& natural_width) const;
      virtual void on_size_allocate(Gtk::Allocation& allocation);
      virtual void on_map();
      virtual void on_unmap();
      virtual void on_realize();
      virtual void on_unrealize();
      virtual bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr);

      virtual bool on_key_press_event(GdkEventKey* event);
      virtual bool on_button_press_event(GdkEventButton* event);
      virtual bool on_button_release_event (GdkEventButton* event);

      void onResize(int x, int y, int width, int height);

      void paint(Canvas& canvas, int width, int height);

      bool mouseToScreen(Point point, int& col, int& row, bool& margin);
      void _captureMouse();
      void _releaseMouse();
      bool _isMouseCaptured()
      {
         return _mouseCaptured;
      }

//   ////size_t      _width, _height;
//
//   //int         _blinkTime;
//   //int         _blinkTimeout;
      int getLineNumberMargin();

      void resizeDocument(int width, int height);
      void update(bool resized);

   public:
      Document*   _document;
      bool        _needToResize;

      ViewStyles  _styles;
      bool        _lineNumbersVisible;
      bool        _mouseCaptured;
      bool        _highlight;
      bool        _tabUsing;
      size_t      _tabSize;

      void onHScroll(int newPosition);
      void onVScroll(int newPosition);
      void onEditorChange();

      void indent();
      void outdent();
////   //void undo();
////   //void redo();
////
      TextDrawingArea(TextView* view);
   };

   TextDrawingArea               _area;
   Gtk::VScrollbar               _vscrollbar;
   Gtk::HScrollbar               _hscrollbar;

   Glib::RefPtr<Gtk::Adjustment> _vadjustment;
   Glib::RefPtr<Gtk::Adjustment> _hadjustment;

   void on_vscroll();
   void on_hscroll();

   virtual void on_grab_focus ()
   {
      _area.grab_focus();
   }

   void onEditorChange();

public:
   int getVScrollerValue() const
   {
      return _vadjustment->get_value();
   }
   int getHScrollerValue() const
   {
      return _hadjustment->get_value();
   }

   void updateHScroller(bool resized);
   void updateVScroller(bool resized);

   bool setScrollerInfo(bool resized, bool vertical, bool horizontal);

   // object methods
   Document* getDocument() { return _area._document; }
   void setDocument(Document* document);

   void setStyles(int count, StyleInfo* styles, int lineHeight, int marginWidth);

   void applySettings(int tabSize, bool tabUsing, bool lineNumberVisible, bool highlight);

   virtual void refreshView()
   {
      _area.onEditorChange();
   }

   TextView();
   //virtual ~TextView();
};

} // _GUI_

#endif // gtktextviewH
