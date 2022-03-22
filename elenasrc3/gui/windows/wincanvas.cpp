//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Win32 Common header
//      Win32 graphic tools body
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "wincanvas.h"

using namespace elena_lang;

// --- Font ----

Font ::  Font()
{
   fontID = nullptr;
   characterSet = /*IDE_CHARSET_DEFAULT*/0;
   size = 8;
   fontName = nullptr;
   bold = italic = false;
}

Font :: Font(wstr_t faceName, int characterSet, int size, bool bold, bool italic)
{
   this->fontName = faceName;
   this->characterSet = characterSet;
   this->size = size;
   this->bold = bold;
   this->italic = italic;

   this->fontID = nullptr;
}

void Font :: create(HDC handler)
{
   LOGFONT lf;
   memset(&lf, 0, sizeof(lf));

   lf.lfHeight = -MulDiv(size, ::GetDeviceCaps(handler, LOGPIXELSY), 72);
   lf.lfWeight = bold ? FW_BOLD : FW_NORMAL;
   lf.lfItalic = (BYTE)(italic ? 1 : 0);
   lf.lfCharSet = (BYTE)(characterSet);

   size_t length = LF_FACESIZE;
   StrConvertor::copy(lf.lfFaceName, fontName, fontName.length(), length);
   lf.lfFaceName[length] = 0;

   fontID = ::CreateFontIndirect(&lf);
}

void Font :: release()
{
   if (fontID != 0) {
      ::DeleteObject(fontID);
      fontID = nullptr;
   }
}

// --- FontFactory ----

FontBase* FontFactory :: createFont(wstr_t fontName, int size, int characterSet, bool bold, bool italic)
{
   for (auto it = _cache.start(); !it.eof(); ++it) {
      Font* font = *it;

      if (font->fontName.compare(fontName) && font->size == size &&
         font->characterSet == characterSet && font->bold == bold && font->italic == italic)
      {
         return font;
      }
   }

   Font* font = new Font(fontName, characterSet, size, bold, italic);
   _cache.add(font);

   return font;
}

// --- Canvas ----

Canvas :: Canvas(HDC handler)
{
   _handler = handler;
   _cloned = false;
   _cloned = false;
   _brush = nullptr;
   _font = nullptr;

   _oldBrush = nullptr;
   _oldBitmap = nullptr;
   _oldFont = nullptr;
   _oldPen = nullptr;

   ::SetTextAlign(_handler, TA_TOP | TA_LEFT);
}

Canvas::Canvas()
{
   _handler = nullptr;
   _cloned = false;
   _brush = nullptr;
   _font = nullptr;

   _oldBrush = nullptr;
   _oldBitmap = nullptr;
   _oldFont = nullptr;
   _oldPen = nullptr;
}

void Canvas :: setBrushColor(Color background)
{
   releaseBrush();

   Color nearestColour = ::GetNearestColor(_handler, background);
   _brush = ::CreateSolidBrush(nearestColour);
   _oldBrush = (HBRUSH)::SelectObject(_handler, _brush);
}

void Canvas :: setPenColor(Color foreground)
{
   releasePen();

   HPEN pen = ::CreatePen(PS_SOLID, 1, foreground);
   _oldPen = (HPEN)::SelectObject(_handler, pen);
}

void Canvas :: setFont(Font* font)
{
   if (font == nullptr) {
      releaseFont();
   }
   else if (!font->fontID || font->fontID != _font) {
      if (_oldFont) {
         ::SelectObject(_handler, font->fontID);
      }
      else _oldFont = (HFONT)::SelectObject(_handler, font->fontID);

      _font = font->fontID;
   }
}

void Canvas :: releaseFont()
{
   if (_font) {
      _font = (HFONT)::SelectObject(_handler, _oldFont);
      _font = nullptr;
      _oldFont = nullptr;
   }
}

void Canvas :: releaseBrush()
{
   if (_oldBrush) {
      _brush = (HBRUSH)::SelectObject(_handler, _oldBrush);
      ::DeleteObject(_brush);
      _oldBrush = nullptr;
      _brush = nullptr;
   }
}

void Canvas :: releasePen()
{
   if (_oldPen) {
      HPEN pen = (HPEN)::SelectObject(_handler, _oldPen);
      ::DeleteObject(pen);
      _oldPen = nullptr;
   }
}

bool Canvas :: isReleased() const
{
   return _handler == nullptr;
}

void Canvas :: clone(Canvas* canvas, int width, int height)
{
   release();

   _cloned = true;
   _handler = ::CreateCompatibleDC(canvas->_handler);

   HBITMAP bitmap = ::CreateCompatibleBitmap(canvas->_handler, width, height);
   _oldBitmap = (HBITMAP)::SelectObject(_handler, bitmap);

   ::SetTextAlign(_handler, TA_TOP | TA_LEFT);
}

void Canvas :: validateStyle(Style* style)
{
   if (!style->valid) {
      Font* font = static_cast<Font*>(style->font);

      if (!font->fontID)
         font->create(_handler);

      setFont(font);
      TEXTMETRIC tm;
      if (::GetTextMetrics(_handler, &tm)) {
         style->lineHeight = tm.tmHeight;
         style->avgCharWidth = tm.tmAveCharWidth;
      }

      style->valid = true;
   }
}

int Canvas :: TextWidth(Style* style, wstr_t s, int length)
{
   setFont((Font*)style->font);
   SIZE sz = { 0,0 };
   ::GetTextExtentPoint32(_handler, s.str(), length, &sz);

   return sz.cx;
}

void Canvas :: drawRectangle(Rectangle rect, Color foreground, Color background)
{
   setPenColor(foreground);
   setBrushColor(background);

   ::Rectangle(_handler, rect.topLeft.x, rect.topLeft.y,
      rect.bottomRight.x, rect.bottomRight.y);
}

void Canvas :: fillRectangle(Rectangle rect, Color background)
{
   setBrushColor(background);

   RECT r = RectFromRectangle(rect);

   ::FillRect(_handler, &r, _brush);
}

void Canvas :: drawEllipse(Rectangle rect, Color foreground, Color background)
{
   setPenColor(foreground);
   setBrushColor(background);

   ::Ellipse(_handler, rect.topLeft.x, rect.topLeft.y,
      rect.bottomRight.x, rect.bottomRight.y);
}

void Canvas :: setTransparentMode(bool on)
{
   if (on)
      ::SetBkMode(_handler, TRANSPARENT);
}

void Canvas :: setClipArea(Rectangle rect)
{
   ::IntersectClipRect(_handler, rect.topLeft.x, rect.topLeft.y,
      rect.bottomRight.x, rect.bottomRight.y);
}

void Canvas :: drawText(Rectangle rect, wstr_t s, int length, Color foreground, bool centered)
{
   RECT r = RectFromRectangle(rect);

   ::SetTextColor(_handler, foreground);

   ::DrawText(_handler, s, length, &r, DT_SINGLELINE | DT_VCENTER | (centered ? DT_CENTER : DT_LEFT));
}

void Canvas :: drawTextClipped(Rectangle rect, Font* font, int x, int y, wstr_t s, int length, Color foreground,
   Color background)
{
   setFont(font);

   ::SetTextColor(_handler, foreground);
   ::SetBkColor(_handler, background);

   RECT r = RectFromRectangle(rect);

   ::ExtTextOut(_handler, x, y, ETO_OPAQUE | ETO_CLIPPED, &r, s.str(), length, nullptr);
}

void Canvas :: drawTextClippedTransporent(Rectangle rect, Font* font, int x, int y, wstr_t s, int length, Color foreground)
{
   setFont(font);

   ::SetTextColor(_handler, foreground);

   RECT r = RectFromRectangle(rect);

   ::ExtTextOut(_handler, x, y, ETO_CLIPPED, &r, s.str(), length, NULL);
}

void Canvas :: copy(Rectangle rect, Point from, Canvas& sour)
{
   ::BitBlt(_handler,
      rect.topLeft.x, rect.topLeft.y, rect.width(), rect.height(),
      sour._handler, from.x, from.y, SRCCOPY);
}

void Canvas::release()
{
   if (_handler) {
      if (_oldBitmap) {
         HBITMAP bitmap = (HBITMAP)::SelectObject(_handler, _oldBitmap);
         ::DeleteObject(bitmap);
         _oldBitmap = nullptr;
      }
      releasePen();
      releaseBrush();
      releaseFont();
      if (_cloned)
         ::DeleteDC(_handler);

      _handler = nullptr;
      _cloned = false;
   }
}

long Canvas :: Chrome()
{
   return ::GetSysColor(COLOR_3DFACE);
}
