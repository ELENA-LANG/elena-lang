//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Win32 Common header
//      GTK graphic tools header
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef gtkgraphicH
#define gtkgraphicH

#include "gtkcommon.h"

namespace _GUI_
{

// --- Colour ---

struct Colour
{
   double red;
   double green;
   double blue;
   double alpha;

   bool operator == (Colour& color)
   {
      if (red == color.red && green == color.green && blue == color.blue && alpha == color.alpha) {
         return true;
      }
      else return false;
   }

   bool operator != (Colour& color)
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

   Colour()
   {
      set(0, 0, 0);
   }
   Colour(double red, double green, double blue)
   {
      set(red, green, blue);
   }

   Colour(double red, double green, double blue, double alpha)
   {
      set(red, green, blue, alpha);
   }
};

// --- Font ---

struct Font
{
   static _ELENA_::Map<const char*, Font*, false> Cache;

   static Font* createFont(const char* fontName);
   static void releaseFontCache();

public:
   Pango::FontDescription _font;

//   void release();

   Font(const char* faceName);
//   ~Font() { release(); }
};

// --- Style ---

struct Style
{
   bool   valid;

   Colour foreground;
   Colour background;
   Font*  font;
   int    lineHeight;
   int    avgCharWidth;

   void validate(Glib::RefPtr<Pango::Layout> layout);

   Style();
   Style(Colour foreground, Colour background, Font* font);
   ~Style();
};

//// --- DoubleBuffer ---
//
//struct Canvas;
//
//struct DoubleBuffer
//{
//   bool initialized;
//
//   cairo_surface_t* cs;
//
//   void init(Canvas& canvas, size_t width, size_t height);
//
//   bool isReleased() { return !initialized; }
//
//   void release()
//   {
//      if (initialized) {
//         initialized = false;
//
//         cairo_surface_destroy(cs);
//      }
//   }
//
//   DoubleBuffer()
//   {
//      initialized = false;
//   }
//};

// --- Canvas ---

struct Canvas
{
   const Cairo::RefPtr<Cairo::Context> cr;
   Glib::RefPtr<Pango::Layout>         layout;

//   void fillRectangle(int x, int y, int width, int height, Style& style);

   void drawText(int x, int y, const char* s, Style& style);
   void drawCursor(int x, int y, Style& style);
   void drawOverwriteCursor(int x, int y, Style& style);

//   void copy(DoubleBuffer& buffer, int x, int y);

//   Canvas(GdkDrawable* drawable);
////   Canvas(DoubleBuffer& buffer);
//   ~Canvas();
//   PangoLayout* layout;

   void fillRectangle(int x, int y, int width, int height, Style& style);

//   void drawText(int x, int y, const TCHAR* s, int length, Style& style);
//   void drawCursor(GtkWidget* widget, int x, int y, Style& style);
//   void drawOverwriteCursor(GtkWidget* widget, int x, int y, Style& style);
//
//   void copy(DoubleBuffer& buffer, int x, int y);
//
   Canvas(const Cairo::RefPtr<Cairo::Context>& cr);
////   Canvas(DoubleBuffer& buffer);
//   ~Canvas();
};

} // _GUI_

#endif // gtkgraphicH

