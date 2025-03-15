//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Win32 Common header
//      GTK graphic tools header
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKGRAPHIC_H
#define GTKGRAPHIC_H

#include "gtklinux/gtkcommon.h"

namespace elena_lang
{

// --- Colour ---

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

// --- Font ---

struct Font : public FontBase
{
public:
   text_str fontName;

   Pango::FontDescription font;

//   void release();

   Font(const char* fontName, int size, bool bold, bool italic);
//   ~Font() { release(); }
};

// --- FontFactory ---
class FontFactory
{
   List<Font*, freeobj> _cache;

public:
   Font* createFont(ustr_t fontName, int size, bool bold, bool italic);

   FontFactory()
      : _cache(nullptr)
   {

   }
};

// --- Style ---

struct Style
{
   bool   valid;

   Color  foreground;
   Color  background;
   Font*  font;
   int    lineHeight;
   int    avgCharWidth;

   void validate(Glib::RefPtr<Pango::Layout> layout);

   Style();
   Style(Color foreground, Color background, Font* font);
   ~Style();
};

////// --- DoubleBuffer ---
////
////struct Canvas;
////
////struct DoubleBuffer
////{
////   bool initialized;
////
////   cairo_surface_t* cs;
////
////   void init(Canvas& canvas, size_t width, size_t height);
////
////   bool isReleased() { return !initialized; }
////
////   void release()
////   {
////      if (initialized) {
////         initialized = false;
////
////         cairo_surface_destroy(cs);
////      }
////   }
////
////   DoubleBuffer()
////   {
////      initialized = false;
////   }
////};

// --- Canvas ---

struct Canvas
{
   const Cairo::RefPtr<Cairo::Context> cr;
   Glib::RefPtr<Pango::Layout>         layout;

   void validateStyle(Style* style);

   void drawText(int x, int y, const char* s, Style* style);
   void drawCursor(int x, int y, Style* style);
//   void drawOverwriteCursor(int x, int y, Style& style);
//
////   void copy(DoubleBuffer& buffer, int x, int y);

   int TextWidth(Style* style, const char* s);

   void fillRectangle(int x, int y, int width, int height, Style* style);

   Canvas(const Cairo::RefPtr<Cairo::Context>& cr);
////   Canvas(DoubleBuffer& buffer);
//   ~Canvas();
};

} // elena_lang

#endif // gtkgraphicH
