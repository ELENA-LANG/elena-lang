//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Common Header File
//                                             (C)2021-2024, by Aleksey Rakov
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
   constexpr int CURSOR_TEXT           = 0;
   constexpr int CURSOR_ARROW          = 1;
   constexpr int CURSOR_SIZEWE         = 2;
   constexpr int CURSOR_SIZENS         = 3;

   // --- Notification types ---
   // NOTE : the systen related notifications must be greater than 0x100
   constexpr int STATUS_NOTIFICATION   = 0x101;
   constexpr int STATUS_SELECTION      = 0x102;
   constexpr int STATUS_COMPLETION     = 0x103;
   constexpr int STATUS_TREEITEM       = 0x104;
   constexpr int CONTEXT_MENU_ON       = 0x105;

   // --- ExtNMHDR ---
   struct StatusNMHDR
   {
      NMHDR              nmhrd;
      int                code;
      NotificationStatus status;
   };

   struct SelectionNMHDR
   {
      NMHDR              nmhrd;
      int                code;
      int                param;
   };

   struct TreeItemNMHDR
   {
      NMHDR              nmhrd;
      int                code;
      size_t             item;
      size_t             param;
   };

   struct CompletionNMHDR
   {
      NMHDR              nmhrd;
      int                code;
      int                param;
   };

   struct ContextMenuNMHDR
   {
      NMHDR              nmhrd;
      int                x, y;
      bool               hasSelection;
   };

   // --- Color ---
   class Color
   {
      long _color;

   public:
      operator long() const { return _color; }

      void set(unsigned int red, unsigned int green, unsigned int blue)
      {
         _color = red | (green << 8) | (blue << 16);
      }

      int red()
      {
         return _color & 0xFF;
      }

      int green()
      {
         return (_color >> 8) & 0xFF;
      }

      int blue()
      {
         return (_color >> 16) & 0xFF;
      }

      Color(unsigned int red, unsigned int green, unsigned int blue)
      {
         set(red, green, blue);
      }

      Color(long colour = 0)
      {
         _color = colour;
      }
   };

   // --- ControlBase ---
   class ControlBase : public GUIControlBase
   {
   protected:
      HWND      _handle;
      wstr_t    _title;

      Rectangle _rect;
      int       _minWidth;
      int       _minHeight;

      virtual void onSetFocus() {}
      virtual void onLoseFocus() {}

   public:
      HWND handle() { return _handle; }
      bool checkHandle(void* param) const override
      {
         return (_handle == (HWND)param);
      }

      HWND getToolTipHandle()
      {
         return TabCtrl_GetToolTips(_handle);
      }

      bool visible() override;

      void showWindow(int cmdShow);

      void show() override;
      void hide() override;

      Rectangle getClientRectangle();
      Rectangle getRectangle() override;
      void setRectangle(Rectangle rec) override;

      void setFocus() override;
      void refresh() override;

      virtual void onDrawItem(DRAWITEMSTRUCT* item) {}
      virtual void onSelChanged() {}
      virtual void onDoubleClick(NMHDR* hdr) {}

      virtual HWND create(HINSTANCE instance, wstr_t className, ControlBase* owner);

      virtual wchar_t* getValue() { return nullptr; }
      virtual void clearValue() {}

      ControlBase(wstr_t title, int x, int y, int width, int height) :
         _handle(nullptr),
         _title(title),
         _rect(x, y, width, height),
         _minWidth(0),
         _minHeight(0)
      {
      }
   };

   // --- EventFormatterBase ---
   class WindowApp;

   class EventFormatterBase
   {
   public:
      virtual void sendMessage(EventBase* event, WindowApp* app) = 0;
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
      virtual bool onClose();

      virtual void setCursor(int type);

   public:
      static ATOM registerClass(HINSTANCE hInstance, WNDPROC proc, wstr_t className, HICON icon, wstr_t menuName, HICON smallIcon, unsigned int style);
      static ATOM registerClass(HINSTANCE hInstance, WNDPROC proc, wstr_t className, HICON icon, wstr_t menuName, HICON smallIcon, 
         HCURSOR cursor, HBRUSH background, unsigned int style);

      static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

      WindowBase(wstr_t title, int width, int height)
         : ControlBase(title, 0, 0, width, height)
      {
      }
   };

   class WindowApp : public GUIApp
   {
   protected:
      HINSTANCE            _instance;
      HWND                 _hwnd;

      wstr_t               _accelerators;

      EventFormatterBase*  _eventFormatter;

      bool initInstance(WindowBase* mainWindow, int cmdShow);

   public:
      int run(GUIControlBase* mainWindow, bool maximized, EventBase* startEvent) override;

      void notify(int id, NMHDR* notification);

      void notify(EventBase* event) override;

      void notify(int messageCode, NotificationStatus status) override;
      void notifySelection(int id, size_t param) override;
      void notifyTreeItem(int id, size_t item, size_t param) override;
      void notifyCompletion(int id, int param) override;
      void notifyContextMenu(int id, short x, short y, bool hasSelection) override;

      WindowApp(HINSTANCE instance, wstr_t accelerators, EventFormatterBase* formatter)
      {
         _instance = instance;
         _hwnd = nullptr;
         _accelerators = accelerators;
         _eventFormatter = formatter;
      }
   };

   void makePathRelative(PathString& path, path_t rootPath);

}

#endif