//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Win32 Common header
//      Win32 graphic tools header
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CANVAS_H
#define CANVAS_H

#include "wincommon.h"

namespace elena_lang
{
   // --- Font ---
   struct Font : public FontBase
   {
      HFONT fontID;

      void create(HDC handler);
      void release();

      Font();
      Font(wstr_t faceName, int characterSet, int size, bool bold, bool italic);
      ~Font() { release(); }
   };

   // --- FontFactory ---
   class FontFactory : public  FontFactoryBase
   {
      List<Font*, freeobj> _cache;

   public:
      FontBase* createFont(wstr_t fontName, int size, int characterSet, bool bold, bool italic) override;

      FontFactory()
         : _cache(nullptr)
      {
         
      }
   };

   // --- Canvas ---
   class Canvas : public CanvasBase
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

      void setBrushColor(Color background);
      void setPenColor(Color foreground);
      void setFont(Font* font);
      void releaseFont();
      void releaseBrush();
      void releasePen();

   public:
      static long Chrome();

      bool isReleased() const;

      void clone(Canvas* canvas, int width, int height);

      void validateStyle(Style* style) override;

      int TextWidth(Style* style, wstr_t s, int length);

      void drawRectangle(Rectangle rect, Color foreground, Color background);
      void drawRectangle(Rectangle rect, Style style)
      {
         drawRectangle(rect, style.foreground, style.background);
      }

      void fillRectangle(Rectangle rect, Color background);
      void fillRectangle(Rectangle rect, Style* style)
      {
         fillRectangle(rect, style->background);
      }

      void drawEllipse(Rectangle rect, Color foreground, Color background);
      void drawEllipse(Rectangle rect, Style style)
      {
         drawEllipse(rect, style.foreground, style.background);
      }

      void setTransparentMode(bool on);

      void setClipArea(Rectangle rect);

      void drawText(Rectangle rect, wstr_t s, int length, Color foreground, bool centered);
      void drawTextClipped(Rectangle rect, Font* font, int x, int y, wstr_t s, int length, Color foreground, Color background);
      void drawTextClipped(Rectangle rect, int x, int y, wstr_t s, int length, Style* style)
      {
         drawTextClipped(rect, (Font*)style->font, x, y, s, length, style->foreground, style->background);
      }
      void drawTextClippedTransporent(Rectangle rect, Font* font, int x, int y, wstr_t s, int length, Color foreground);
      void drawTextClippedTransporent(Rectangle rect, int x, int y, wstr_t s, int length, Style style)
      {
         drawTextClippedTransporent(rect, (Font*)style.font, x, y, s, length, style.foreground);
      }

      void copy(Rectangle rect, Point from, Canvas& sour);

      void release();

      Canvas(HDC handler);
      Canvas();
      virtual ~Canvas() { release(); }
   };

   inline RECT RectFromRectangle(Rectangle rect)
   {
      RECT rc = { rect.topLeft.x, rect.topLeft.y, rect.bottomRight.x, rect.bottomRight.y };
      return rc;
   }

}

#endif
