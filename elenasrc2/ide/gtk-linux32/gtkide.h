//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Linux-GTK IDE
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winideH
#define winideH

#include "gtk-linux32/gtksdi.h"
#include "../ide.h"

namespace _GUI_
{
// --- GTKIDEView ---

class MainWindow : public SDIWindow
{
   _Controller*  _controller;
   Model*        _model;

   Gtk::Toolbar* _toolbar;
   EditFrame*    _mainFrame;

protected:
   // event signals
   void on_menu_file_new_source()
   {
      _controller->doCreateFile();
   }
   void on_menu_file_new_project()
   {
      _controller->doCreateProject();
   }
   void on_menu_file_open_source()
   {
      _controller->doOpenFile();
   }
   void on_menu_file_open_project()
   {
      _controller->doOpenProject();
   }
   void on_menu_file_quit()
   {
      _controller->doExit();
   }
////   virtual void _onNotify(NMHDR* notification);
////   void _onToolTip(NMTTDISPINFO* toolTip);

   void populateMenu();
   void populateToolbar();

public:
   int newDocument(const char* name, Document* doc);

   MainWindow(const char* caption, _Controller* controller, Model* model);
};

////// --- Win32ContextBrowser ---
////
////class Win32ContextBrowser : public ContextBrowser
////{
////   ContextMenu _menu;
////
////public:
////   TreeViewItem hitTest(short x, short y);
////
////   void showContextMenu(HWND owner, short x, short y);
////
////   Win32ContextBrowser(Control* owner);
////};
////
////// --- AppDebugController ---
////
////class Win32AppDebugController : public AppDebugController
////{
////   Window* _receptor;
////
////   void _notify(int code);
////   void _notify(int code, const TCHAR* message, int param = 0);
////   void _notify(int code, const TCHAR* message, int param1, int param2);
////   void _notify(int code, const TCHAR* source, HighlightInfo info);
////
////public:
////   void assign(Window* receptor)
////   {
////      _receptor = receptor;
////   }
////
////   virtual void onStart()
////   {
////      _notify(IDE_DEBUGGER_START);
////   }
////
////   virtual void onStop(bool failed)
////   {
////      _notify(failed ? IDE_DEBUGGER_BREAK : IDE_DEBUGGER_STOP);
////   }
////
////   virtual void onStep(const TCHAR* source, int row, int disp, int length)
////   {
////      _notify(IDE_DEBUGGER_STEP, source, HighlightInfo(row, disp, length));
//   }
//
//   void onCheckPoint(const TCHAR* message)
//   {
//      _notify(IDE_DEBUGGER_CHECKPOINT, message);
//   }
//
//   void onNotification(const TCHAR* message, size_t address, int code)
//   {
//      _notify(IDM_DEBUGGER_EXCEPTION, message, address, code);
//   }
//
//   virtual void onLoadModule(const TCHAR* name)
//   {
//      _notify(IDE_DEBUGGER_LOADMODULE, name);
//   }
//
//   Win32AppDebugController()
//   {
//      _receptor = NULL;
//   }
//};

} // _GUI_

#endif // winideH
