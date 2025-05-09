//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI graphical tools Implementation
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtklinux/gtkgraphic.h"

using namespace elena_lang;

// --- Font ---
//
//Font :: Font()
//{
//   _descr = NULL;
//
//   _fontName = NULL;
//}

Font :: Font(const char* fontName, int size, bool bold, bool italic)
{
   this->fontName = fontName;
   this->bold = bold;
   this->italic = italic;
   this->size = size;

   this->font.set_family(fontName);
   this->font.set_size(size * PANGO_SCALE);
   if (bold)
      this->font.set_weight(Pango::WEIGHT_BOLD);

   if (italic)
      this->font.set_style(Pango::STYLE_ITALIC);
}

//void Font :: release()
//{
//   if (_descr != NULL) {
//      pango_font_description_free(_descr);
//      _descr = NULL;
//   }
//}

// --- FontFactory ---

Font* FontFactory:: createFont(ustr_t fontName, int size, bool bold, bool italic)
{
   for (auto it = _cache.start(); !it.eof(); ++it) {
      Font* font = *it;

      if (font->fontName.compare(fontName) && font->size == size &&
         font->bold == bold && font->italic==italic)
      {
         return font;
      }
   }
   Font* font = new Font(fontName, size, bold, italic);
   _cache.add(font);

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

// --- Style ---

Style :: Style()
{
   font = nullptr;
   valid = false;
   avgCharWidth = 0;
   lineHeight = 0;
}

Style :: Style(Color foreground, Color background, Font* font)
{
   this->foreground = foreground;
   this->background = background;
   this->font = font;
   this->valid = false;
   this->avgCharWidth = 8;
   this->lineHeight = 0;
}

Style :: ~Style()
{
}

void Style :: validate(Glib::RefPtr<Pango::Layout> layout)
{
   if (!valid) {
      layout->set_font_description(font->font);
      layout->set_text("H");
      layout->get_pixel_size(avgCharWidth, lineHeight);

      //avgCharWidth >>= 1;
      //avgCharWidth++;

      valid = true;
   }
}

////// --- DoubleBuffer ---
////
////void DoubleBuffer :: init(Canvas& canvas, size_t width, size_t height)
////{
////   cs = cairo_surface_create_similar(cairo_get_target(canvas.cr), CAIRO_CONTENT_COLOR_ALPHA, width, height);
////
////   initialized = true;
////}

// --- Canvas ---

Canvas :: Canvas(const Cairo::RefPtr<Cairo::Context>& context)
   : cr(context)
{
//   this->drawable = drawable;
   this->layout = Pango::Layout::create (cr);
}

void Canvas :: validateStyle(Style* style)
{
}

//////Canvas :: Canvas(DoubleBuffer& buffer)
//////{
//////   this->drawable = NULL;
//////   this->cr = cairo_create(buffer.cs);
//////   this->layout = pango_cairo_create_layout(cr);
//////}
////
////Canvas :: ~Canvas()
////{
//////   g_object_unref(layout);
//////   cairo_destroy(cr);
////}
////
////void Canvas :: copy(DoubleBuffer& buffer, int x, int y)
////{
////   cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
////   cairo_set_source_surface(cr, buffer.cs, x, y);
////   cairo_paint(cr);
////}

void Canvas :: fillRectangle(int x, int y, int width, int height, Style* style)
{
   cr->rectangle (x, y, width, height);
   cr->set_source_rgb(style->background.red, style->background.green, style->background.blue);
   cr->fill();
}

void Canvas :: drawText(int x, int y, const char* s, Style* style)
{
   cr->set_source_rgb(style->foreground.red, style->foreground.green, style->foreground.blue);

   cr->move_to(x, y);

   layout->set_font_description(style->font->font);
   layout->set_text(s);

   layout->update_from_cairo_context(cr);
   layout->show_in_cairo_context(cr);
}

int Canvas :: TextWidth(Style* style, const char* s)
{
   layout->set_font_description(style->font->font);

   layout->set_text(s);

   int text_width;
   int text_height;
   layout->get_pixel_size(text_width, text_height);

   return text_width;
}

void Canvas :: drawCursor(int x, int y, Style* style)
{
   cr->set_source_rgb(style->foreground.red, style->foreground.green, style->foreground.blue);

   cr->move_to(x, y);
   cr->line_to(x, y + style->lineHeight);

   cr->stroke();
}

//void Canvas :: drawOverwriteCursor(int x, int y, Style& style)
//{
////   GdkRectangle cursor_location;
////
////   cursor_location.x = x;
////   cursor_location.y = y + style.lineHeight;
////   cursor_location.width = style.avgCharWidth;
////   cursor_location.height = 2;
////
////   gdk_cairo_rectangle(cr, &cursor_location);
////   cairo_set_source_rgb(cr, style.foreground.red, style.foreground.green, style.foreground.blue);
////   cairo_fill (cr);
//}
//
