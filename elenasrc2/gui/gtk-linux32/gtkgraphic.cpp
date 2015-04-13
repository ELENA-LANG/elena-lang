//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI graphical tools Implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "gtkgraphic.h"

using namespace _GUI_;

// --- Font ---

_ELENA_::Map<const char*, Font*, false> Font :: Cache = _ELENA_::Map<const char*, Font*, false>(NULL, _ELENA_::freeobj);

Font* Font :: createFont(const char* fontName)
{
   Font* font = Cache.get(fontName);
   if (font == NULL) {
      font = new Font(fontName);

      Cache.add(fontName, font);
   }
   return font;
}

//void Font :: releaseFontCache()
//{
//   _ELENA_::List<Font*>::Iterator it = Cache.start();
//   while (!it.Eof()) {
//      Font* font = *it;
//      it++;
//
//      Cache.cut(font);
//   }
//}
//
//Font :: Font()
//{
//   _descr = NULL;
//
//   _fontName = NULL;
//}

Font :: Font(const char* fontName)
   : _font(fontName)
{
}

//void Font :: release()
//{
//   if (_descr != NULL) {
//      pango_font_description_free(_descr);
//      _descr = NULL;
//   }
//}

// --- Style ---

Style :: Style()
{
   font = NULL;
   valid = false;
   avgCharWidth = 0;
   lineHeight = 0;
}

Style :: Style(Colour foreground, Colour background, Font* font)
{
   this->foreground = foreground;
   this->background = background;
   this->font = font;
   this->valid = false;
   this->avgCharWidth = 0;
   this->lineHeight = 0;
}

Style :: ~Style()
{
}

void Style :: validate(Glib::RefPtr<Pango::Layout> layout)
{
   if (!valid) {
      layout->set_font_description(font->_font);
      layout->set_text("H");
      layout->get_pixel_size(avgCharWidth, lineHeight);

      //avgCharWidth >>= 1;
      //avgCharWidth++;

      valid = true;
   }
}

//// --- DoubleBuffer ---
//
//void DoubleBuffer :: init(Canvas& canvas, size_t width, size_t height)
//{
//   cs = cairo_surface_create_similar(cairo_get_target(canvas.cr), CAIRO_CONTENT_COLOR_ALPHA, width, height);
//
//   initialized = true;
//}

// --- Canvas ---

Canvas :: Canvas(const Cairo::RefPtr<Cairo::Context>& context)
   : cr(context)
{
//   this->drawable = drawable;
   this->layout = Pango::Layout::create (cr);
}

////Canvas :: Canvas(DoubleBuffer& buffer)
////{
////   this->drawable = NULL;
////   this->cr = cairo_create(buffer.cs);
////   this->layout = pango_cairo_create_layout(cr);
////}
//
//Canvas :: ~Canvas()
//{
////   g_object_unref(layout);
////   cairo_destroy(cr);
//}
//
//void Canvas :: copy(DoubleBuffer& buffer, int x, int y)
//{
//   cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
//   cairo_set_source_surface(cr, buffer.cs, x, y);
//   cairo_paint(cr);
//}

void Canvas :: fillRectangle(int x, int y, int width, int height, Style& style)
{
   cr->rectangle (x, y, width, height);
   cr->set_source_rgb(style.background.red, style.background.green, style.background.blue);
   cr->fill();
}

void Canvas :: drawText(int x, int y, const char* s, Style& style)
{
   cr->set_source_rgb(style.foreground.red, style.foreground.green, style.foreground.blue);

   cr->move_to(x, y);
//
//   //style.font->_descr = pango_font_description_from_string(style.font->_fontName);

   layout->set_font_description(style.font->_font);
   layout->set_text(s);

//   //pango_font_description_free(style.font->_descr);

   layout->update_from_cairo_context(cr);
   layout->show_in_cairo_context(cr);

   //pangoLayout->add_to_cairo_context(cr);       //adds text to cairos stack of stuff to be drawn
   //cr->stroke();
}

void Canvas :: drawCursor(int x, int y, Style& style)
{
   cr->set_source_rgb(style.foreground.red, style.foreground.green, style.foreground.blue);

   cr->move_to(x, y);
   cr->line_to(x, y + style.lineHeight);

   cr->stroke();
}

void Canvas :: drawOverwriteCursor(int x, int y, Style& style)
{
//   GdkRectangle cursor_location;
//
//   cursor_location.x = x;
//   cursor_location.y = y + style.lineHeight;
//   cursor_location.width = style.avgCharWidth;
//   cursor_location.height = 2;
//
//   gdk_cairo_rectangle(cr, &cursor_location);
//   cairo_set_source_rgb(cr, style.foreground.red, style.foreground.green, style.foreground.blue);
//   cairo_fill (cr);
}

