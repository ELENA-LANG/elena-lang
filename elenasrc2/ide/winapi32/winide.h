//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Win32 IDE
//
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winideH
#define winideH

#include "winapi32\wincommon.h"
#include "winapi32\winsdi.h"
#include "winapi32\winmenu.h"
#include "winapi32\wintoolbar.h"
#include "winapi32\winstatusbar.h"
#include "winapi32\winsplitter.h"
#include "debugger.h"
#include "winoutput.h"
#include "wineditframe.h"
#include "winoutput.h"
#include "winideconst.h"
#include "..\appwindow.h"

namespace _GUI_
{
// -- WIN32IDE ---

class MainWindow;

class WIN32IDE : public IDE
{
   friend class MainWindow;

   HINSTANCE   instance;
   ContextMenu contextMenu;

   Output*     _output;

   _ELENA_::List<_BaseControl*> controls;

   TabBar* createOutputBar();
   ContextBrowser* createContextBrowser();

protected:
   virtual void onProjectClose();

   virtual bool compileProject(int postponedAction);

   bool isBrowser(HWND handle)
   {
      return _contextBrowser->checkHandle(handle);
   }

   const wchar16_t* getDocumentPath(int index)
   {
      return _mainFrame->getDocumentPath(index);
   }

public:
   void start(bool maximized);

   virtual void onCustomDraw(void* handle, void* item);

   virtual void onDebuggerStep(const wchar16_t* ns, const wchar_t* source, HighlightInfo info);
   virtual void onDebuggerStop(bool broken);

   void onDoubleClick(NMHDR* notification)
   {
      if (_messageList->getHandle()==notification->hwndFrom) {
         highlightMessage(_messageList->getBookmark(((LPNMITEMACTIVATE)notification)->iItem));
      }
   }

   void onTVItemExpanded(NMTREEVIEW* notification)
   {
      if (_contextBrowser->getHandle()==notification->hdr.hwndFrom) {
         _contextBrowser->browse(_debugController, notification->itemNew.hItem);
      }
   }

   void onRClick(NMHDR* notification);
   void onChildKeyDown(NMHDR* notification);
   void onClientChanged(int code, NMHDR* notification);
   void onContextMenu(ContextMenuNMHDR* notification);
   void onTabChanged(HWND wnd, int index);
   void onActivate();

   void onDebuggerVMHook()
   {
      _debugController->loadBreakpoints(_breakpoints);
   }

   Menu* getMainMenu()
   {
      return _appMenu;
   }

   SDIWindow* getAppWindow()
   {
      return _appWindow;
   }

   void displayErrors();

   void cleanUpProject();

   virtual void openHelp();

   WIN32IDE(HINSTANCE instance, AppDebugController* debugController);
};

// --- MainWindow ---

class MainWindow : public SDIWindow
{
   friend class WIN32IDE;

   WIN32IDE* _ide;
   HWND      _tabTTHandle;      

protected:
   virtual void onActivate();
   virtual bool onClose();

   virtual void _onNotify(NMHDR* notification);
   virtual void _onMenuCommand(int id);
   virtual void _onDrawItem(DRAWITEMSTRUCT* item);

   void _onToolTip(NMTTDISPINFO* toolTip);
   void _onTabTip(NMTTDISPINFO* toolTip);
   void _onChildKeyDown(NMHDR* notification);

   bool isTabToolTip(HWND handle)
   {
      if (!_tabTTHandle)
         _tabTTHandle = _ide->_mainFrame->getToolTipHandle();

      return _tabTTHandle == handle;
   }

public:
   MainWindow(HINSTANCE instance, const wchar_t* caption, WIN32IDE* env);
};

// --- Win32ContextBrowser ---

class Win32ContextBrowser : public ContextBrowser
{
   ContextMenu _menu;

public:
   TreeViewItem hitTest(short x, short y);

   void showContextMenu(HWND owner, short x, short y);

   Win32ContextBrowser(Control* owner);
};

// --- AppDebugController ---

class Win32AppDebugController : public AppDebugController
{
   Window* _receptor;
   size_t  _vmHook;

   void _notify(int code);
   void _notify(int code, const wchar_t* message, int param = 0);
   void _notify(int code, const wchar_t* message, const wchar_t* param);
   void _notify(int code, const wchar_t* message, int param1, int param2);
   void _notify(int code, const wchar_t* ns, const wchar_t* source, HighlightInfo info);

   virtual void clearDebugInfo()
   {
      _vmHook = 0;

      AppDebugController::clearDebugInfo();
   }

   virtual size_t findEntryPoint(const tchar_t* programPath);

public:
   void assign(Window* receptor)
   {
      _receptor = receptor;
   }

   virtual void onStart()
   {
      _notify(IDE_DEBUGGER_START);
   }

   virtual void onStop(bool failed)
   {
      _notify(failed ? IDE_DEBUGGER_BREAK : IDE_DEBUGGER_STOP);
   }

   virtual void onStep(const wchar_t* ns, const wchar_t* source, int row, int disp, int length)
   {
      _notify(IDE_DEBUGGER_STEP, ns, source, HighlightInfo(row, disp, length));
   }

   void onCheckPoint(const wchar_t* message)
   {
      _notify(IDE_DEBUGGER_CHECKPOINT, message);
   }

   void onNotification(const wchar_t* message, size_t address, int code)
   {
      _notify(IDM_DEBUGGER_EXCEPTION, message, address, code);
   }

   virtual void onLoadModule(const wchar_t* name, const wchar16_t* path)
   {
      _notify(IDE_DEBUGGER_LOADMODULE, name, path);
   }

   virtual void onLoadTape(const wchar_t* name, int tapePtr)
   {
      _notify(IDE_DEBUGGER_LOADTEMPMODULE, name, tapePtr);
   }

   virtual void onInitBreakpoint();

   Win32AppDebugController()
   {
      _receptor = NULL;
      _vmHook = 0;
   }
};

} // _GUI_

#endif // winideH
