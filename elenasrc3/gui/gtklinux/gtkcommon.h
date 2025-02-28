//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI common classes header File
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GUICOMMON_H
#define GUICOMMON_H

#include "common.h"

namespace elena_lang
{
#ifdef _MSC_VER

   typedef wide_c          text_c;
   typedef wide_c*         text_t;
   typedef const wide_c*   const_text_t;
   typedef wstr_t          text_str;

#elif __GNUG__

#ifndef _T

   #define _T(x) x

#endif // _T

   typedef char         text_c;
   typedef char*        text_t;
   typedef const char*  const_text_t;
   typedef ustr_t       text_str;

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
      virtual bool checkHandle(void* param) const = 0;

      virtual Rectangle getRectangle() = 0;
      virtual void setRectangle(Rectangle rec) = 0;

      virtual void show() = 0;
      virtual void hide() = 0;
      virtual bool visible() = 0;

      virtual void setFocus() = 0;

      virtual void refresh() = 0;

      virtual ~GUIControlBase() = default;
   };

   // --- GUIMenuBase ---
   class GUIMenuBase : public GUIControlBase
   {
   public:
      virtual wchar_t getMnemonicAccKey() = 0;

      virtual void checkMenuItemById(int id, bool checked) = 0;

      virtual void insertSeparatorById(int positionId, int id) = 0;
      virtual void insertMenuItemById(int positionId, int id, const_text_t caption) = 0;
      virtual void insertMenuItemByIndex(int index, int command, const_text_t caption) = 0;

      virtual void enableMenuItemById(int id, bool doEnable) = 0;
      virtual void enableMenuItemByIndex(int index, bool doEnable) = 0;

      virtual void eraseMenuItemById(int id) = 0;

      virtual void renameMenuItemById(int id, const_text_t caption) =0 ;
   };

   // --- NotifierBase ---
   class EventBase
   {
   public:
      int status;

      virtual int eventId() = 0;

      EventBase(int status)
         : status(status)
      {

      }
   };

   class NotifierBase
   {
   public:
      virtual void notify(EventBase* event) = 0;
   };

   // --- GUIApp ---
   class GUIApp : public NotifierBase
   {
   public:
      virtual int run(GUIControlBase* mainWindow, bool maximized, EventBase* startEvent) = 0;

      virtual ~GUIApp() = default;
   };

   // --- ConstantIdentifier ---
   class TextString : public String <text_c, 0x100>
   {
   public:
      text_str operator*() const { return text_str(_string); }

      TextString()
      {
         _string[0] = 0;
      }
      TextString(ustr_t message)
      {
         size_t length = 0x100;
         message.copyTo(_string, length);
         _string[length] = 0;
      }
   };
}

#endif
