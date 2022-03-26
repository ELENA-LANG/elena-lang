//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Common Header File
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINCOMMON_H
#define WINCOMMON_H

#include "guicommon.h"

#ifndef _WIN32_IE
#define _WIN32_IE 0x500
#endif
#define _WIN32_WINNT 0x500

#ifndef WINVER
#define WINVER 0x0500
#endif

#include <windows.h>
#include <commctrl.h>

namespace elena_lang
{
   // --- Cursor types ---
   constexpr int CURSOR_TEXT     = 0;
   constexpr int CURSOR_ARROW    = 1;
   constexpr int CURSOR_SIZEWE   = 2;
   constexpr int CURSOR_SIZENS   = 3;

   constexpr int NMHDR_Message   = 0x101;
   constexpr int NMHDR_Model     = 0x102;

   // --- ExtNMHDR ---
   struct ExtNMHDR
   {
      NMHDR nmhrd;
      int   extParam;
   };

   // --- ControlBase ---
   class ControlBase : public GUIControlBase
   {
   protected:
      HWND   _handle;
      wstr_t _title;

      virtual void onSetFocus() {}
      virtual void onLoseFocus() {}

   public:
      HWND handle() { return _handle; }
      bool checkHandle(void* param) const
      {
         return (_handle == (HWND)param);
      }

      bool visible() override;

      void showWindow(int cmdShow);

      Rectangle getRectangle();
      virtual void setRectangle(Rectangle rec);

      virtual void setFocus();
      virtual void refresh();

      virtual void onDrawItem(DRAWITEMSTRUCT* item) {}

      virtual HWND create(HINSTANCE instance, wstr_t className, ControlBase* owner);

      ControlBase(wstr_t title)
      {
         _handle = nullptr;
         _title = title;
      }
   };

   // --- WindowBase ---
   class WindowBase : public ControlBase
   {
   protected:
      void assignHandle(HWND handle)
      {
         _handle = handle;
      }

      virtual LRESULT proceed(UINT message, WPARAM wParam, LPARAM lParam);

      virtual void onResize() {}
      virtual bool onSetCursor() { return false; }

      virtual void setCursor(int type);

   public:
      static ATOM registerClass(HINSTANCE hInstance, WNDPROC proc, wstr_t className, HICON icon, wstr_t menuName, HICON smallIcon, unsigned int style);

      static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

      WindowBase(wstr_t title)
         : ControlBase(title)
      {
      }
   };

   class WindowApp : public GUIApp
   {
   protected:
      HINSTANCE _instance;
      HWND      _hwnd;

      int       _cmdShow;
      wstr_t    _accelerators;

      bool initInstance(WindowBase* mainWindow);

   public:
      int run(GUIControlBase* mainWindow) override;

      void notifyMessage(int messageCode) override;
      void notifyModelChange(int modelCode) override;

      WindowApp(HINSTANCE instance, int cmdShow, wstr_t accelerators)
      {
         _instance = instance;
         _hwnd = nullptr;
         _cmdShow = cmdShow;
         _accelerators = accelerators;
      }
   };

}

#endif