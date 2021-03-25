//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI Common Header File
//                                              (C)2005-2016, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef guicommonH
#define guicommonH

#include "common.h"

#ifdef _WIN32

typedef const wchar_t* text_t;
typedef wchar_t        text_c;

typedef _ELENA_::wide_t text_str;

#define _T(x) L ## x

#elif _LINUX

typedef const char* text_t;
typedef char        text_c;

typedef _ELENA_::ident_t text_str;

#define _T(x) x

#endif

#define DEFAULT_TEXT (text_t)NULL

namespace _GUI_
{

// --- Point ---

struct Point
{
   int x;
   int y;

   Point operator +(const Point point) const
   {
      return Point(this->x + point.x, this->y + point.y);
   }

   Point operator -(const Point point) const
   {
      return Point(this->x - point.x, this->y - point.y);
   }

   Point& operator -=(const Point point)
   {
	  this->x -= point.x;
	  this->y -= point.y;

	  return *this;
   }

   Point& operator +=(const Point point)
   {
	  this->x += point.x;
	  this->y += point.y;

	  return *this;
   }

   bool operator ==(const Point point) const
   {
      return (this->x == point.x && this->y == point.y);
   }

   bool operator !=(const Point point) const
   {
      return (this->x != point.x || this->y != point.y);
   }

   Point()
   {
      x = 0;
      y = 0;
   }
   Point(int x, int y)
   {
      this->x = x;
      this->y = y;
   }
};

// --- Rectangle ---

struct Rectangle
{
   Point topLeft;
   Point bottomRight;

   int Width() const { return bottomRight.x - topLeft.x + 1; }
   int Height() const { return bottomRight.y - topLeft.y + 1; }

   bool isWithIn(Point point)
   {
      return topLeft.x <= point.x && topLeft.y <= point.y &&
		  point.x <= bottomRight.x && point.y <= bottomRight.y;
   }

   Rectangle()
   {
   }
   Rectangle(const Rectangle& rectangle)
   {
      topLeft = rectangle.topLeft;
      bottomRight = rectangle.bottomRight;
   }
   Rectangle(int left, int right, int width, int height)
   {
      topLeft = Point(left, right);
      bottomRight = topLeft + Point(width - 1, height - 1);
   }
};

// --- HighlightInfo ---
struct HighlightInfo
{
   int row;
   int col;
   int disp;
   int length;

   HighlightInfo()
   {
      row = disp = length = 0;
   }
   HighlightInfo(int row, int disp, int length)
   {
      this->row = row;
      this->disp = disp;
      this->length = length;
      this->col = 0;
   }
   HighlightInfo(int col, int row, int disp, int length)
   {
      this->col = col;
      this->row = row;
      this->disp = disp;
      this->length = length;
   }
};

// --- ConstantIdentifier ---

class TextString : public _ELENA_::String <text_c, 0x100>
{
public:
   text_str str() { return text_str(_string); }

   TextString(_ELENA_::ident_t message)
   {
      size_t length = 0x100;
      message.copyTo(_string, length);
      _string[length] = 0;
   }
};

} // _GUI_

#endif // guicommonH
