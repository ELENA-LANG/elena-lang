//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Win32 IDE
//
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winideH
#define winideH

//#include "winapi32\wincommon.h"
#include "winapi32\winsdi.h"
#include "winapi32\winmenu.h"
#include "winapi32\wintoolbar.h"
#include "winapi32\wintreeview.h"
#include "..\ide.h"
#include "..\windowlist.h"
#include "..\historylist.h"
#include "..\browser.h"

//#include "debugger.h"

#define TABCHANGED_NOTIFY  1
#define VIEWCHANGED_NOTIFY 2

namespace _GUI_
{
//// -- WIN32IDE ---
//
//class MainWindow;
//
//class WIN32IDE : public IDE
//{
//   friend class MainWindow;
//
//   HINSTANCE   instance;
//   ContextMenu contextMenu;
//
//   _ELENA_::List<_BaseControl*> controls;
//
//   TabBar* createOutputBar();
//   ContextBrowser* createContextBrowser();
//
//protected:
//
//   virtual bool compileProject(int postponedAction);
//
//   bool isBrowser(HWND handle)
//   {
//      return _contextBrowser->checkHandle(handle);
//   }
//
//   const wchar16_t* getDocumentPath(int index)
//   {
//      return _mainFrame->getDocumentPath(index);
//   }
//
//public:
//   void start(bool maximized);
//
//   virtual void onCustomDraw(void* handle, void* item);
//
//   virtual void onDebuggerStep(const wchar16_t* ns, const wchar_t* source, HighlightInfo info);
//   virtual void onDebuggerStop(bool broken);
////
//   void onChildKeyDown(NMHDR* notification);
//   void onContextMenu(ContextMenuNMHDR* notification);
//   void onTabChanged(HWND wnd, int index);
//   void onActivate();
//
//   Menu* getMainMenu()
//   {
//      return _appMenu;
//   }
//
//   SDIWindow* getAppWindow()
//   {
//      return _appWindow;
//   }
//
//   void displayErrors();
//
//   void cleanUpProject();
//
//   virtual void openHelp();
//
//   WIN32IDE(HINSTANCE instance, AppDebugController* debugController);
//};

// --- MainWindow ---

class MainWindow : public SDIWindow
{
   class ContextBrowser : public _Browser
   {
      DebugWatch*  _watch;
      ContextMenu  _menu;
      Control*     _treeView;
      Model*       _model;

   public:
      virtual bool isHexNumberMode();

      void reset();
      void refresh(_ELENA_::_DebugController* controller);

      virtual bool isExpanded(void* node);
      virtual void expand(void* node);
      virtual void clear(void* node);
      virtual void erase(void* node);

      virtual void getCaption(void* node, _ELENA_::ident_c* caption, size_t length);
      virtual void setCaption(void* node, _ELENA_::ident_t caption);

      virtual void setParam(void* node, size_t param);
      virtual size_t getParam(void* node);

      virtual void* getCurrent();

      virtual void* newNode(void* parent, _ELENA_::ident_t caption, int param);
      virtual void* findNodeStartingWith(void* node, _ELENA_::ident_t caption);

      void assign(Control* treeView);

      TreeViewItem hitTest(short x, short y);
      void showContextMenu(HWND owner, short x, short y, Model* model);

      void browse(_ELENA_::_DebugController* controller);
      void browse(_ELENA_::_DebugController* controller, void* current);

      ContextBrowser(Model* model);
      ~ContextBrowser() { _ELENA_::freeobj(_watch); }
   };

   _Controller* _controller;
   Model*       _model;
   HWND         _tabTTHandle;

protected:
   Clipboard      _clipboard;
   ContextBrowser _contextBrowser;

   size_t         _controlCount;
   _BaseControl** _controls;

   WindowList     _windowList;
   RecentList     _recentFiles;
   RecentList     _recentProjects;

   void setLeft(int index);
   void setTop(int index);
   void setClient(int index);
   void setBottom(int index);

   void showControls(int from, int till);
   void hideControl(int index);
   void refreshControl(int index);

   virtual void onActivate();
   virtual bool onClose();

   virtual void _onNotify(NMHDR* notification);
   virtual void _onMenuCommand(int id);
   virtual void _onDrawItem(DRAWITEMSTRUCT* item);
   void onClientChanged(int code, NMHDR* notification);
   void onTabChanged(HWND wnd, int index);
   void onContextMenu(ContextMenuNMHDR* notification); 
   void onDoubleClick(NMHDR* notification);
   void onRClick(NMHDR* notification);
   void onDebuggerStep(LineInfoNMHDR* notification);
   void onDebuggerStop(bool failed);

   void _onToolTip(NMTTDISPINFO* toolTip);
   void _onTabTip(NMTTDISPINFO* toolTip);
   void _onChildKeyDown(NMHDR* notification);   
   void onTVItemExpanded(NMTREEVIEW* notification);

   bool isTabToolTip(HWND handle);

   void openHelp();
   void displayErrors();

   bool checkControlHandle(int index, HWND wnd);
   bool isControlVisible(int index);

public:
   Menu* getMenu();
   ToolBar* getToolBar();

   void showFrame();
   void activateFrame();
   void hideFrame();
   void refreshDocument();

   int newDocument(text_t name, Document* doc);
   int getCurrentDocumentIndex();
   void selectDocument(int docIndex);
   void markDocumentTitle(int docIndex, bool changed);
   void renameDocument(int index, text_t name);
   void closeDocument(int docIndex);

   void setStatusBarText(int index, text_t message);

   void addToWindowList(const wchar_t* path);
   void removeFromWindowList(const wchar_t* path);

   void addToRecentFileList(const wchar_t* path);
   void addToRecentProjectList(const wchar_t* path);

   void reloadSettings();

   void loadHistory(_ELENA_::IniConfigFile& file, const char* recentFileSection, const char* recentProjectSection)
   {
      _recentFiles.load(file, recentFileSection);
      _recentFiles.refresh();

      _recentProjects.load(file, recentProjectSection);
      _recentProjects.refresh();
   }

   void saveHistory(_ELENA_::IniConfigFile& file, const char* recentFileSection, const char* recentProjectSection)
   {
      _recentFiles.save(file, recentFileSection);

      _recentProjects.save(file, recentProjectSection);
   }

   bool copyToClipboard(Document* document);
   void pasteFrameClipboard(Document* document);

   void openOutput();
   void switchToOutput();
   void closeOutput();

   void openMessageList();
   void clearMessageList();
   void closeMessageList();

   void openDebugWatch();
   void closeDebugWatch();

   void openCallList();
   void closeCallList();

   bool compileProject(_ProjectManager* manager, int postponedAction);

   void _notify(int code);
   void _notify(int code, const wchar_t* message, int param = 0);
   void _notify(int code, const wchar_t* message, const wchar_t* param);
   void _notify(int code, const wchar_t* message, int param1, int param2);
   void _notify(int code, const wchar_t* ns, const wchar_t* source, HighlightInfo info);

   void resetDebugWindows();
   void refreshDebugWindows(_ELENA_::_DebugController* debugController);
   void browseWatch(_ELENA_::_DebugController* debugController, void* watchNode);
   void browseWatch(_ELENA_::_DebugController* debugController);
   
   MainWindow(HINSTANCE instance, const wchar_t* caption, _Controller* controller, Model* model);
   virtual ~MainWindow();
};
//
//// --- AppDebugController ---
//
//class Win32AppDebugController : public AppDebugController
//{
//   Window* _receptor;
//
//   virtual void clearDebugInfo()
//   {
//      _vmHook = 0;
//
//      AppDebugController::clearDebugInfo();
//   }
//
//   virtual size_t findEntryPoint(const tchar_t* programPath);
//
//public:
//   void assign(Window* receptor)
//   {
//      _receptor = receptor;
//   }
//
//   virtual void onLoadModule(const wchar_t* name, const wchar16_t* path)
//   {
//      _notify(IDE_DEBUGGER_LOADMODULE, name, path);
//   }
//
//   virtual void onLoadTape(const wchar_t* name, int tapePtr)
//   {
//      _notify(IDE_DEBUGGER_LOADTEMPMODULE, name, tapePtr);
//   }
//
//   Win32AppDebugController()
//   {
//      _receptor = NULL;
//      _vmHook = 0;
//   }
//};

} // _GUI_

#endif // winideH
