//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Win32 Common header
//      Win32 graphic tools header
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef wingraphicH
#define wingraphicH

#include "wincommon.h"

namespace _GUI_
{

#define IDE_CHARSET_ANSI                        0
#define IDE_CHARSET_DEFAULT                     1

// --- Colour ---

class Colour
{
   long _colour;

public:
   operator long() const { return _colour; }

   void set(unsigned int red, unsigned int green, unsigned int blue)
   {
      _colour = red | (green << 8) | (blue << 16);
   }

   Colour(unsigned int red, unsigned int green, unsigned int blue)
   {
      set(red, green, blue);
   }

   Colour(long colour = 0)
   {
	  _colour = colour;
   }
};

// --- Font ---

struct Font
{
   static _ELENA_::List<Font*> Cache;

   static Font* createFont(const wchar_t* fontName, int characterSet, int size,
		                        bool bold, bool italic);
   static void releaseFontCache();

public:
   HFONT        _fontID;

   text_str     _fontName;
   bool         _bold;
   bool         _italic;
   int          _size;
   int          _characterSet;

   HFONT ID() const { return _fontID; }

   void create(HDC handler);
   void release();

   Font();
   Font(const wchar_t* faceName, int characterSet, int size, bool bold, bool italic);
   ~Font() { release(); }
};

// --- Style ---

class Canvas;

struct Style
{
   bool   valid;

   Colour foreground;
   Colour background;
   Font*  font;
   int    lineHeight;
   int    avgCharWidth;

   void validate(Canvas* canvas);

   Style();
   Style(Colour foreground, Colour background, Font* font);
   ~Style();
};

// --- Canvas ---

class Canvas
{
protected:
   HDC     _handler;
   bool    _cloned;

   HBRUSH  _brush;
   HFONT   _font;
   HBRUSH  _oldBrush;
   HBITMAP _oldBitmap;
   HFONT   _oldFont;
   HPEN    _oldPen;

   void setBrushColor(Colour background);
   void setPenColour(Colour foreground);
   void setFont(Font* font);

   void releaseFont();
   void releaseBrush();
   void releasePen();

public:
   static long Chrome();
   static long ButtonFace();
   static long ButtonShadow();

   bool isReleased() const;

   void clone(Canvas* canvas, int width, int height);

   bool validateStyle(Style* style);

   int TextWidth(Style* style, const wchar_t* s, int length);

   void drawRectangle(Rectangle rect, Colour foreground, Colour background);
   void drawRectangle(Rectangle rect, Style style)
   {
      drawRectangle(rect, style.foreground, style.background);
   }

   void fillRectangle(Rectangle rect, Colour background);
   void fillRectangle(Rectangle rect, Style style)
   {
      fillRectangle(rect, style.background);
   }

   void drawEllipse(Rectangle rect, Colour foreground, Colour background);
   void drawEllipse(Rectangle rect, Style style)
   {
      drawEllipse(rect, style.foreground, style.background);
   }

   void setTransparentMode(bool on);

   void setClipArea(Rectangle rect);
   void drawText(Rectangle rect, const wchar_t* s, int length, Colour foreground, bool centered);
   void drawTextClipped(Rectangle rect, Font* font, int x, int y, const wchar_t* s, int length, Colour foreground, Colour background);
   void drawTextClipped(Rectangle rect, int x, int y, const wchar_t* s, int length, Style style)
   {
      drawTextClipped(rect, style.font, x, y, s, length, style.foreground, style.background);
   }
   void drawTextClippedTransporent(Rectangle rect, Font* font, int x, int y, const wchar_t* s, int length, Colour foreground);
   void drawTextClippedTransporent(Rectangle rect, int x, int y, const wchar_t* s, int length, Style style)
   {
      drawTextClippedTransporent(rect, style.font, x, y, s, length, style.foreground);
   }

   void copy(Rectangle rect, Point from, Canvas& sour);

   void release();

   Canvas();
   Canvas(HDC handler);
   virtual ~Canvas() { release(); }
};

inline RECT RectFromRectangle(_GUI_::Rectangle rect)
{
   RECT rc = {rect.topLeft.x, rect.topLeft.y, rect.bottomRight.x, rect.bottomRight.y};
   return rc;
}

} // _GUI_

#endif // wingraphicH