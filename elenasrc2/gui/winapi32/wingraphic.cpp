//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI graphical tools Implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "wingraphic.h"

using namespace _GUI_;

// --- Font ---

_ELENA_::List<Font*> Font :: Cache = _ELENA_::List<Font*>(NULL, _ELENA_::freeobj);

Font* Font :: createFont(const wchar_t* fontName, int characterSet, int size,
		                bool bold, bool italic)
{
   _ELENA_::List<Font*>::Iterator it = Cache.start();
   while (!it.Eof()) {
      Font* font = *it;

      if (font->_fontName.compare(fontName) && font->_size == size &&
         font->_characterSet==characterSet && font->_bold == bold && font->_italic==italic)
      {
         return font;
      }
      it++;
   }
   Font* font = new Font(fontName, characterSet, size, bold, italic);

   Cache.add(font);

   return font;
}

void Font :: releaseFontCache()
{
   _ELENA_::List<Font*>::Iterator it = Cache.start();
   while (!it.Eof()) {
      Font* font = *it;
      it++;

      Cache.cut(font);
   }
}

Font :: Font()
{
   _fontID = NULL;

   _characterSet = IDE_CHARSET_DEFAULT;
   _size = 8;
}

Font :: Font(const wchar_t* fontName, int characterSet, int size, bool bold, bool italic)
{
   _fontID = NULL;

   _fontName = fontName;
   _characterSet = characterSet;
   _size = size;
   _bold = bold;
   _italic = italic;
}

void Font :: create(HDC handler)
{
   LOGFONT lf;

   memset(&lf, 0, sizeof(lf));
   lf.lfHeight = -MulDiv(_size, ::GetDeviceCaps(handler, LOGPIXELSY), 72);
   lf.lfWeight = _bold ? FW_BOLD : FW_NORMAL;
   lf.lfItalic = (BYTE)(_italic ? 1 : 0);
   lf.lfCharSet = (BYTE)(_characterSet);

   size_t length = LF_FACESIZE;
   _ELENA_::Convertor::copy(lf.lfFaceName, _fontName, _ELENA_::getlength(_fontName), length);
   lf.lfFaceName[length] = 0;

   _fontID = ::CreateFontIndirect(&lf);
}

void Font :: release()
{
   if (_fontID != 0) {
      ::DeleteObject(_fontID);
      _fontID = 0;
   }
}

// --- Style ---

Style :: Style()
{
   font = NULL;
   valid = false;
   avgCharWidth = 8;
   lineHeight = 0;
}

Style :: Style(Colour foreground, Colour background, Font* font)
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

void Style :: validate(Canvas* canvas)
{
   if (!valid) {
      if (canvas->validateStyle(this))
         valid = true;
   }
}

// --- Canvas ---

Canvas :: Canvas()
{
   _handler = NULL;
   _oldBitmap = NULL;
   _oldBrush = NULL;
   _brush = NULL;
   _oldPen = NULL;
   _font = NULL;
   _oldFont = NULL;
   _cloned = false;
}

Canvas :: Canvas(HDC handler)
{
   _handler = handler;
   _oldBitmap = NULL;
   _oldBrush = NULL;
   _brush = NULL;
   _oldPen = NULL;
   _font = NULL;
   _oldFont = NULL;
   _cloned = false;

   ::SetTextAlign(_handler, TA_TOP | TA_LEFT);
}

bool Canvas :: isReleased() const
{
   return (_handler == NULL);
}

void Canvas :: setTransparentMode(bool on)
{
   if (on)
      ::SetBkMode(_handler, TRANSPARENT);
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

bool Canvas :: validateStyle(Style* style)
{
   if (!style->font->ID()) {
      style->font->create(_handler);
   }
   setFont(style->font);
   TEXTMETRIC tm;
   if (::GetTextMetrics(_handler, &tm)) {
      style->lineHeight = tm.tmHeight;
      style->avgCharWidth = tm.tmAveCharWidth;

      return true;
   }
   else return false;
}

int Canvas :: TextWidth(Style* style, const wchar_t* s, int length)
{
   setFont(style->font);
   SIZE sz={0,0};
   ::GetTextExtentPoint32(_handler, s, length, &sz);

   return sz.cx;
}

void Canvas :: drawRectangle(_GUI_::Rectangle rect, Colour foreground, Colour background)
{
   setPenColour(foreground);
   setBrushColor(background);

   ::Rectangle(_handler, rect.topLeft.x, rect.topLeft.y,
               rect.bottomRight.x, rect.bottomRight.y);
}

void Canvas :: drawEllipse(_GUI_::Rectangle rect, Colour foreground, Colour background)
{
   setPenColour(foreground);
   setBrushColor(background);

   ::Ellipse(_handler, rect.topLeft.x, rect.topLeft.y,
             rect.bottomRight.x, rect.bottomRight.y);
}

void Canvas :: fillRectangle(_GUI_::Rectangle rect, Colour background)
{
   setBrushColor(background);

   RECT r = RectFromRectangle(rect);

   ::FillRect(_handler, &r, _brush);
}

void Canvas :: setClipArea(_GUI_::Rectangle rect)
{
   ::IntersectClipRect(_handler, rect.topLeft.x, rect.topLeft.y, 
	   rect.bottomRight.x, rect.bottomRight.y);
}

void Canvas :: drawText(_GUI_::Rectangle rect, const wchar_t* s, int length, Colour foreground, bool centered)
{
   RECT r = RectFromRectangle(rect);

   ::SetTextColor(_handler, foreground);

   ::DrawText(_handler, s, (int)_ELENA_::getlength(s), &r, DT_SINGLELINE | DT_VCENTER | (centered ? DT_CENTER : DT_LEFT));
}

void Canvas :: drawTextClipped(_GUI_::Rectangle rect, Font* font, int x, int y, const wchar_t* s, int length, Colour foreground, Colour background)
{
   setFont(font);

   ::SetTextColor(_handler, foreground);
   ::SetBkColor(_handler, background);

   RECT r = RectFromRectangle(rect);

   ::ExtTextOut(_handler, x, y, ETO_OPAQUE | ETO_CLIPPED, &r, s, length, NULL);
}

void Canvas :: drawTextClippedTransporent(_GUI_::Rectangle rect, Font* font, int x, int y, const wchar_t* s, int length, Colour foreground)
{
   setFont(font);

   ::SetTextColor(_handler, foreground);

   RECT r = RectFromRectangle(rect);

   ::ExtTextOut(_handler, x, y, ETO_CLIPPED, &r, s, length, NULL);
}

void Canvas :: setFont(Font* font)
{
   if (font==NULL) {
      releaseFont();
   }
   else if (!font->ID() || font->ID() != _font) {
      if (_oldFont) {
         ::SelectObject(_handler, font->ID());
      }
      else {
         _oldFont = (HFONT)::SelectObject(_handler, font->ID());
      }
      _font = (HFONT)font->ID();
   }
}

void Canvas :: setBrushColor(Colour background)
{
   releaseBrush();

   Colour nearestColour = ::GetNearestColor(_handler, background);
   _brush = ::CreateSolidBrush(nearestColour);
   _oldBrush = (HBRUSH)::SelectObject(_handler, _brush);
}

void Canvas :: setPenColour(Colour foreground)
{
   releasePen();

   HPEN pen = ::CreatePen(PS_SOLID, 1, foreground);
   _oldPen = (HPEN)::SelectObject(_handler, pen);
}

void Canvas :: releaseFont()
{
   if (_font) {
      _font = (HFONT)::SelectObject(_handler, _oldFont);
      _font = NULL;
      _oldFont = NULL;
   }
}

void Canvas :: releaseBrush()
{
   if (_oldBrush) {
      _brush = (HBRUSH)::SelectObject(_handler, _oldBrush);
      ::DeleteObject(_brush);
      _oldBrush = NULL;
	  _brush = NULL;
   }
}

void Canvas :: releasePen()
{
   if (_oldPen) {
      HPEN pen = (HPEN)::SelectObject(_handler, _oldPen);
      ::DeleteObject(pen);
      _oldPen = NULL;
   }
}

void Canvas :: copy(_GUI_::Rectangle rect, Point from, Canvas& sour)
{
   ::BitBlt(_handler,
		rect.topLeft.x, rect.topLeft.y, rect.Width(), rect.Height(),
		sour._handler, from.x, from.y, SRCCOPY);
}

void Canvas :: release()
{
   if (_handler) {
      if (_oldBitmap) {
         HBITMAP bitmap = (HBITMAP)::SelectObject(_handler, _oldBitmap);
         ::DeleteObject(bitmap);
         _oldBitmap = NULL;
      }
      releasePen();
      releaseBrush();
      releaseFont();
      if (_cloned)
         ::DeleteDC(_handler);

      _handler = NULL;
   }
}

long Canvas :: Chrome()
{
   return ::GetSysColor(COLOR_3DFACE);
}

long Canvas :: ButtonFace()
{
   return ::GetSysColor(COLOR_BTNFACE);
}

long Canvas :: ButtonShadow()
{
   return ::GetSysColor(COLOR_BTNHILIGHT);
}
