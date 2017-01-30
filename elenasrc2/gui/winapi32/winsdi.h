//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI SDI Control Header File
//                                               (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winsdiH
#define winsdiH

#include "wincommon.h"
#include "winmenu.h"

namespace _GUI_
{

// --- WinAPI Window Classes ---
#define APP_WND_CLASS                           _T("ELENA APP CLASS")

// --- LayoutManager ---

class LayoutManager
{
   Control* _top;
   Control* _left;
   Control* _right;
   Control* _bottom;
   Control* _client;

public:
   Control* getClient() const { return _client; }
   Control* getBottom() const { return _bottom; }

   void setAsTop(Control* control) { _top = control; }
   void setAsBottom(Control* control) { _bottom = control; }
   void setAsLeft(Control* control) { _left = control; }
   void setAsRight(Control* control) { _right = control; }
   void setAsClient(Control* control) { _client = control; }

   void resizeTo(Rectangle area);

   LayoutManager()
   {
      _top = NULL;
      _left = NULL;
      _right = NULL;
      _bottom = NULL;
      _client = NULL;
   } 
};

// --- SDIWindow ---

class SDIWindow : public Window
{
protected:
   LayoutManager _layoutManager;

   Control* _statusBar;     

   void _onResizing(RECT* rect);
   virtual void onResize();
   virtual void onActivate();
   virtual void _onMenuCommand(int id) = 0;
   virtual void _onNotify(NMHDR* notification) = 0;
   virtual bool _onSetCursor();   
   virtual void _onDrawItem(DRAWITEMSTRUCT* item);
   
   virtual LRESULT _WindProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

public:
   static void _registerClass(HINSTANCE instance, HICON icon, wchar_t* menu);

   virtual void show() { Control::show(); }
   virtual void show(bool maximized);
   virtual void exit();

   virtual void refresh()
   {
      Window::refresh();

      onResize();
   }

   SDIWindow(HINSTANCE instance, const wchar_t* caption);
};

} // _GUI_

#endif // winsdiH
