//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Common Header File
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef wincommonH
#define wincommonH

#ifndef _WIN32_IE
#define _WIN32_IE 0x500
#endif
#define _WIN32_WINNT 0x500

#ifndef WINVER
#define WINVER 0x0500
#endif

#include <windows.h>
#include <commctrl.h>
#include <shlwapi.h>

#include "..\guicommon.h"

// --- Cursors ---

#define CURSOR_TEXT                             0
#define CURSOR_ARROW                            1
#define CURSOR_SIZEWE						         2
#define CURSOR_SIZENS                           3

namespace _GUI_
{

// --- _BaseControl ---

class _BaseControl
{
public:
   virtual bool checkHandle(void* param) const = 0;

   virtual ~_BaseControl() {}
};

// --- Control ---

class Control : public _BaseControl
{
protected:
   HINSTANCE _instance;
   HWND      _handle;

   int       _left;
   int       _top;
   int       _width;
   int       _height;

   int       _minWidth;
   int       _minHeight;

public:
   virtual bool checkHandle(void* param) const
   {
      return (_handle == (HWND)param);
   }

   HWND getToolTipHandle()
   {
      return TabCtrl_GetToolTips(_handle);
   }

   HWND getHandle() const { return _handle; }
   HINSTANCE _getInstance() const { return _instance; }

   virtual bool isVisible();

   virtual int getLeft() const { return _left; }
   virtual int getTop() const { return _top; }
   virtual int getWidth() const { return _width; }
   virtual int getHeight() const { return _height; }
   virtual Rectangle getRectangle();

   virtual void _setCoordinate(int x, int y);
   virtual void _setConstraint(int minWidth, int minHeight);
   virtual void _setWidth(int width);
   virtual void _setHeight(int height);

   virtual void setFocus();
   virtual void show();
   virtual void hide();
   virtual void refresh();

   virtual void _resize();

   virtual void _onDrawItem(DRAWITEMSTRUCT* item) {}

   void _notify(HWND receptor, int code);
   void _notify(HWND receptor,   int code, int extParam);

   Control(int left, int top, int width, int height);
   virtual ~Control() {}
};

// --- Window ---

class Window : public Control
{
protected:
   virtual LRESULT _WindProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

   virtual bool onClose();
   virtual void onResize() {} 
   virtual bool _onSetCursor() { return false; }   
   virtual void onSetFocus() {}
   virtual void onLoseFocus() {}

   void _setCursor(int type);

public:
   void setCaption(const wchar_t* caption);

   static void _registerClass(HINSTANCE hInstance, const wchar_t* name, UINT style, HCURSOR cursor = NULL, 
                             HBRUSH background = NULL, HICON icon = NULL, wchar_t* menu = NULL);

   static LRESULT CALLBACK _Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

   Window(int left, int top, int width, int height)
      : Control(left, top, width, height)
   {
   }
};

// --- Clipboard ---

class Clipboard
{
public:
   static bool isAvailable();

   bool begin(Window* window)
   {
      return open(window->getHandle());
   }

   void end()
   {
      close();
   }

   bool open(HWND id);
   HGLOBAL create(size_t size);
   wchar_t* allocate(HGLOBAL buffer);
   void free(HGLOBAL buffer);
   void copy(HGLOBAL buffer);
   size_t getSize(HGLOBAL buffer);
   HGLOBAL get();

   void clear();
   void close();
};

// --- DateTime ---

struct DateTime
{
private:
   SYSTEMTIME _time;

public:
   static DateTime getFileTime(const wchar_t* path);

   bool operator > (const DateTime dt) const
   {
      if (_time.wYear > dt._time.wYear)
         return true;

      if (_time.wYear==dt._time.wYear) {
         if (_time.wMonth > dt._time.wMonth)
            return true;

         if (_time.wMonth==dt._time.wMonth) {
            if (_time.wDay > dt._time.wDay)
               return true;

            if (_time.wDay==dt._time.wDay) {
               if (_time.wHour > dt._time.wHour)
                  return true;

               if (_time.wHour==dt._time.wHour) {
                  if (_time.wMinute > dt._time.wMinute)
                     return true;

                  if (_time.wMinute==dt._time.wMinute) {
                     if (_time.wSecond > dt._time.wSecond)
                        return true;

                     if (_time.wSecond==dt._time.wSecond) {
                        return (_time.wMilliseconds > dt._time.wMilliseconds);
                     }
                  }
               }
            }
         }
      }
      return false;
   }

   DateTime()
   {
      _time.wYear = 0;
      //memset(&_time, 0, sizeof(_time));
   }
};

// --- misc functions ---

inline bool isPathRelative(const wchar_t* path)
{
   return (::PathIsRelative(path) == TRUE);
}

inline void canonicalize(_ELENA_::Path& path)
{
   wchar_t p[MAX_PATH];

   ::PathCanonicalize(p, path);

   path.copy(p);
}

inline void makePathRelative(_ELENA_::Path& path, const wchar_t* rootPath)
{
   wchar_t tmpPath[MAX_PATH];

   ::PathRelativePathTo(tmpPath, rootPath, FILE_ATTRIBUTE_DIRECTORY, path, FILE_ATTRIBUTE_NORMAL);
   if (!_ELENA_::emptystr(tmpPath)) {
      if (_ELENA_:: wide_t(tmpPath).compare(_T(".\\"), 2)) {
         path.copy(tmpPath + 2);
      }
      else path.copy(tmpPath);
   }
}

// --- ExtNMHDR ---

struct ExtNMHDR
{
   NMHDR nmhrd;
   int   extParam;
};

// --- ContextMenuNMHDR ---

struct ContextMenuNMHDR
{
   NMHDR nmhrd;
   int   x, y;
};

// --- MessageNMHDR ---

struct MessageNMHDR
{
   NMHDR        nmhrd;
   const wchar_t* message;
   int          param;
};

struct Message2NMHDR
{
   NMHDR        nmhrd;
   const wchar_t* message;
   size_t       param1;
   int          param2;
};

struct Message3NMHDR
{
   NMHDR          nmhrd;
   const wchar_t* message;
   const wchar_t* param;
};

// --- LineInfoNMHDR ---

struct LineInfoNMHDR
{
   NMHDR             nmhrd;
   const wchar_t*    ns;
   const wchar_t*    file;
   HighlightInfo     position;
};

} // _GUI_

#endif // wincommonH