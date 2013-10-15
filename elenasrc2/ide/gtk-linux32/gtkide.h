//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Linux-GTK IDE
//
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winideH
#define winideH

//#include "winapi32\wincommon.h"
//#include "winapi32\winsdi.h"
//#include "winapi32\winmenu.h"
//#include "winapi32\wintoolbar.h"
//#include "winapi32\winstatusbar.h"
//#include "winapi32\winsplitter.h"
//#include "debugger.h"
//#include "winoutput.h"
//#include "wineditframe.h"
//#include "winoutput.h"
//#include "winideconst.h"
#include "../appwindow.h"

namespace _GUI_
{
// -- GTKIDE ---

class MainWindow;

class GTKIDE : public IDE
{
   friend class MainWindow;

////   ContextMenu contextMenu;
////
////   Output*     _output;

   _ELENA_::List<void*> controls;

//   Menu* createMainMenu();
////   TabBar* createOutputBar();
////   ContextBrowser* createContextBrowser();
////
protected:
   virtual void onIDEInit();

//   virtual void onProjectClose();

   virtual bool compileProject(int postponedAction)
   {
      return false; // !! temporal
   }

public:
   void start(bool maximized);

////   virtual void onCustomDraw(void* handle, void* item);
////
////   virtual void onDebuggerStep(const TCHAR* source, HighlightInfo info);
////   virtual void onDebuggerStop(bool broken);
////
////   void onDoubleClick(NMHDR* notification)
////   {
////      if (_messageList->getHandle()==notification->hwndFrom) {
////         highlightMessage(_messageList->getBookmark(((LPNMITEMACTIVATE)notification)->iItem));
////      }
////   }
////
////   void onRClick(NMHDR* notification);
////   void onChildKeyDown(NMHDR* notification);
////
////   Menu* getMainMenu()
////   {
////      return _appMenu;
////   }

////   void displayErrors();
////
////   void cleanUpProject();

   virtual void openHelp() {}

   GTKIDE();
};

// --- MainWindow ---

class MainWindow : public SDIWindow
{
   GTKIDE* _ide;

protected:
   // event signals
   void on_menu_file_new_source()
   {
      _ide->doCreateFile();
   }

   void on_menu_file_quit()
   {
      _ide->doExit();
   }
//   virtual void _onNotify(NMHDR* notification);
//   void _onToolTip(NMTTDISPINFO* toolTip);

public:
   MainWindow(const _text_t* caption, GTKIDE* env);
};

//// --- Win32ContextBrowser ---
//
//class Win32ContextBrowser : public ContextBrowser
//{
//   ContextMenu _menu;
//
//public:
//   TreeViewItem hitTest(short x, short y);
//
//   void showContextMenu(HWND owner, short x, short y);
//
//   Win32ContextBrowser(Control* owner);
//};
//
//// --- AppDebugController ---
//
//class Win32AppDebugController : public AppDebugController
//{
//   Window* _receptor;
//
//   void _notify(int code);
//   void _notify(int code, const TCHAR* message, int param = 0);
//   void _notify(int code, const TCHAR* message, int param1, int param2);
//   void _notify(int code, const TCHAR* source, HighlightInfo info);
//
//public:
//   void assign(Window* receptor)
//   {
//      _receptor = receptor;
//   }
//
//   virtual void onStart()
//   {
//      _notify(IDE_DEBUGGER_START);
//   }
//
//   virtual void onStop(bool failed)
//   {
//      _notify(failed ? IDE_DEBUGGER_BREAK : IDE_DEBUGGER_STOP);
//   }
//
//   virtual void onStep(const TCHAR* source, int row, int disp, int length)
//   {
//      _notify(IDE_DEBUGGER_STEP, source, HighlightInfo(row, disp, length));
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
