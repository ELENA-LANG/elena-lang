//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA Win32 IDE
//
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winideH
#define winideH

////#include "winapi32\wincommon.h"
#include "winapi32\winsdi.h"
#include "winapi32\winmenu.h"
#include "winapi32\wintoolbar.h"
#include "winapi32\wintreeview.h"
#include "..\ide.h"
#include "..\windowlist.h"
#include "..\historylist.h"
#include "..\browser.h"

#define TABCHANGED_NOTIFY  1
#define VIEWCHANGED_NOTIFY 2

namespace _GUI_
{

// --- IDEWindow ---

class IDEWindow : public SDIWindow, public _View, public _DebugListener
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

      virtual void getCaption(void* node, char* caption, size_t length);
      virtual void setCaption(void* node, _ELENA_::ident_t caption);

      virtual void setParam(void* node, size_t param);
      virtual size_t getParam(void* node);

      virtual void* getCurrent();

      virtual void* newNode(void* parent, _ELENA_::ident_t caption, size_t param);
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
   void setRight(int index);
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
   void onIndexChange(NMHDR* notification);
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
   virtual void start(bool maximized)
   {
      show(maximized);
   }

   virtual void showStatus(int index, text_t message)
   {
      setStatusBarText(index, message);
   }   

   virtual void enableMenuItemById(int id, bool doEnable, bool toolBarItemAvailable)
   {
      getMenu()->enableItemById(id, doEnable);
      if (toolBarItemAvailable)
         getToolBar()->enableItemById(id, doEnable);
   }

   virtual void checkMenuItemById(int id, bool doEnable)
   {
      getMenu()->checkItemById(id, doEnable);
   }

   virtual void exit()
   {
      SDIWindow::exit();
   }

   virtual void setCaption(text_t caption)
   {
      SDIWindow::setCaption(caption);
   }

   virtual void showFrame();

   virtual void refresh(bool onlyFrame)
   {
      if (onlyFrame) {
         refreshDocument();
      }
      else SDIWindow::refresh();
   }

   Menu* getMenu();
   ToolBar* getToolBar();

   virtual void activateFrame();
   void hideFrame();
   void refreshDocument();

   virtual int newDocument(text_t name, Document* doc);
   virtual int getCurrentDocumentIndex();
   virtual void selectDocument(int docIndex);
   virtual void markDocumentTitle(int docIndex, bool changed);
   virtual void renameDocument(int index, text_t name);
   void closeDocument(int docIndex);

   void setStatusBarText(int index, text_t message);

   virtual bool selectFiles(Model* model, _ELENA_::List<text_c*>& selected);
   virtual bool selectProject(Model* model, _ELENA_::Path& path);
   virtual bool saveProject(Model* model, _ELENA_::Path& path);
   virtual bool saveFile(Model* model, _ELENA_::Path& newPath);
   virtual bool confirm(text_t message);
   virtual bool confirm(text_t message, text_t param1, text_t param2);
   virtual _View::Answer question(text_t message);
   virtual _View::Answer question(text_t message, text_t param);
   virtual void error(text_t message);
   virtual void error(text_t message, text_t param);
   virtual bool find(Model* model, SearchOption* option, SearchHistory* searchHistory);
   virtual bool replace(Model* model, SearchOption* option, SearchHistory* searchHistory, SearchHistory* replaceHistory);
   virtual bool gotoLine(int& row);
   virtual bool selectWindow(Model* model, _Controller* controller);

   virtual bool configProject(_ProjectManager* project);
   virtual bool configEditor(Model* model);
   virtual bool configDebugger(Model* model);
   virtual bool configurateForwards(_ProjectManager* project);
   virtual bool about(Model* model);

   virtual void addToWindowList(const wchar_t* path);
   virtual void removeFromWindowList(const wchar_t* path);

   virtual void addToRecentFileList(const wchar_t* path);
   virtual void addToRecentProjectList(const wchar_t* path);

   virtual void reloadSettings();

   virtual void removeFile(_ELENA_::path_t name);

   void loadHistory(_ELENA_::XmlConfigFile& file, const char* recentFileSection, const char* recentProjectSection)
   {
      _recentFiles.load(file, recentFileSection);
      _recentFiles.refresh();

      _recentProjects.load(file, recentProjectSection);
      _recentProjects.refresh();
   }

   void saveHistory(_ELENA_::XmlConfigFile& file, const char* recentFileSection, const char* recentProjectSection)
   {
      _recentFiles.save(file, recentFileSection);

      _recentProjects.save(file, recentProjectSection);
   }

   virtual bool copyToClipboard(Document* document);
   virtual void pasteFromClipboard(Document* document);

   void openTab(const wchar_t* caption, int ctrl);
   void switchToTab(int ctrl);
   void closeTab(int ctrl);

   virtual void openOutput();
   virtual void switchToOutput();
   virtual void closeOutput();

   virtual void openVMConsole(_ProjectManager* project);
//   void switchToVMConsole();
   virtual void closeVMConsole();

   virtual void openMessageList();
   virtual void clearMessageList();
   virtual void closeMessageList();

   virtual void openDebugWatch();
   virtual void closeDebugWatch();

   virtual void openCallList();
   virtual void closeCallList();

   virtual void openProjectView();
   virtual void closeProjectView();

   bool compileProject(_ProjectManager* manager, int postponedAction);

   virtual void onNotification(const wchar_t* message, size_t address, int code)
   {
      _notify(IDM_DEBUGGER_EXCEPTION, message, address, code);
   }
   
   virtual void onStep(_ELENA_::ident_t ns, _ELENA_::ident_t source, int row, int disp, int length)
   {
      _notify(IDE_DEBUGGER_STEP, TextString(ns), TextString(source), HighlightInfo(row, disp, length));
   }
   
   virtual void onLoadTape(_ELENA_::ident_t name, int row, int disp, int length)
   {
      _notify(IDE_DEBUGGER_LOADTEMPMODULE, TextString(name), NULL, HighlightInfo(row, disp, length));
   }

   virtual void onDebuggerHook()
   {
      _notify(IDE_DEBUGGER_HOOK);
   }   
   
   virtual void onStart()
   {
      _notify(IDE_DEBUGGER_START);
   }

   virtual void onStop(bool failed)
   {
      _notify(failed ? IDE_DEBUGGER_BREAK : IDE_DEBUGGER_STOP);
   }

   void onCheckPoint(const wchar_t* message)
   {
      _notify(IDE_DEBUGGER_CHECKPOINT, message);
   }

   void _notify(int code);
   void _notify(int code, const wchar_t* message, int param = 0);
   void _notify(int code, const wchar_t* message, const wchar_t* param);
   void _notify(int code, const wchar_t* message, size_t param1, int param2);
   void _notify(int code, const wchar_t* ns, const wchar_t* source, HighlightInfo info);

//   void resetDebugWindows();
   virtual void refreshDebugWindows(_ELENA_::_DebugController* debugController);
   virtual void browseWatch(_ELENA_::_DebugController* debugController, void* watchNode);
   virtual void browseWatch(_ELENA_::_DebugController* debugController);

   virtual void reloadProjectView(_ProjectManager* project);

   IDEWindow(HINSTANCE instance, const wchar_t* caption, _Controller* controller, Model* model);
   virtual ~IDEWindow();
};

} // _GUI_

#endif // winideH
