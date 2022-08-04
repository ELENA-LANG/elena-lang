//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI common classes header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GUICOMMON_H
#define GUICOMMON_H

#include "common.h"

namespace elena_lang
{
#ifdef _MSC_VER

#ifndef _T

#define _T(x) L ## x

#endif // _T

   typedef wide_c  text_c;
   typedef wide_c* text_t;
   typedef wstr_t  text_str;

#elif __GNUG__

#ifndef _T

   #define _T(x) x

#endif // _T

   typedef char   text_c;
   typedef char*  text_t;
   typedef ustr_t text_str;

#endif

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
         x = y = 0;
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

      int width() const { return bottomRight.x - topLeft.x + 1; }
      int height() const { return bottomRight.y - topLeft.y + 1; }

      void setWidth(int width)
      {
         bottomRight.x = topLeft.x + width - 1;
      }
      void setHeight(int height)
      {
         bottomRight.y = topLeft.y + height - 1;
      }

      Rectangle() = default;
      Rectangle(int left, int top, int width, int height)
      {
         topLeft = Point(left, top);

         bottomRight.x = left + width - 1;
         bottomRight.y = top + height - 1;
      }
   };

   // --- FontBase ---
   struct FontBase
   {
      bool   bold;
      bool   italic;
      int    size;
      int    characterSet;
   };

   // --- GUIControlBase ---
   class GUIControlBase
   {
   public:
      virtual bool visible() = 0;

      virtual ~GUIControlBase() = default;
   };

   // --- NotifierBase ---
   class NotifierBase
   {
   public:
      virtual void notifyMessage(int messageCode) = 0;
      virtual void notifyModelChange(int modelCode, int arg = 0) = 0;
   };

   // --- GUIApp ---
   class GUIApp : public NotifierBase
   {
   public:
      virtual int run(GUIControlBase* mainWindow) = 0;

      virtual ~GUIApp() = default;
   };

}

#endif