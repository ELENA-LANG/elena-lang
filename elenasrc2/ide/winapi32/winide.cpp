//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//    WinAPI: IDE & IDE Controls
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "winide.h"
#include "winideconst.h"

#include "winapi32\winstatusbar.h"
#include "winapi32\wintabbar.h"
#include "wineditframe.h"
#include "winapi32\winsplitter.h"
#include "winoutput.h"
#include "winapi32\winlistview.h"
#include "../settings.h"

using namespace _GUI_;

#define CTRL_MENU          1
#define CTRL_CONTEXTMENU   2
#define CTRL_STATUSBAR     3
#define CTRL_TOOLBAR       4
#define CTRL_EDITFRAME     5
#define CTRL_TABBAR        6
#define CTRL_OUTPUT        7
#define CTRL_MESSAGELIST   8
#define CTRL_CALLLIST      9
#define CTRL_BSPLITTER     10
#define CTRL_HSPLITTER     11
#define CTRL_CONTEXTBOWSER 12
#define CTRL_PROJECTVIEW   13

int AppToolBarButtonNumber = 19;

ToolBarButton AppToolBarButtons[] =
{
   {IDM_FILE_NEW, IDR_FILENEW},
   {IDM_FILE_OPEN, IDR_FILEOPEN},
   {IDM_FILE_SAVE, IDR_FILESAVE},
   {IDM_FILE_SAVEALL, IDR_SAVEALL},
   {IDM_FILE_CLOSE, IDR_CLOSEFILE},
   {IDM_PROJECT_CLOSE, IDR_CLOSEALL},
   {0, IDR_SEPARATOR},
   {IDM_EDIT_CUT, IDR_CUT},
   {IDM_EDIT_COPY, IDR_COPY},
   {IDM_EDIT_PASTE, IDR_PASTE},
   {0, IDR_SEPARATOR},
   {IDM_EDIT_UNDO, IDR_UNDO},
   {IDM_EDIT_REDO, IDR_REDO},
   {0, IDR_SEPARATOR},
   {IDM_DEBUG_RUN, IDR_RUN},
   {IDM_DEBUG_STEPINTO, IDR_STEPINTO},
   {IDM_DEBUG_STEPOVER, IDR_STEPOVER},
   {IDM_DEBUG_STOP, IDR_STOP},
   {IDM_DEBUG_GOTOSOURCE, IDR_GOTO},
};

int StatusBarWidths[5] = {200, 90, 60, 40, 80};

MenuInfo contextMenuInfo[8] = {
   {IDM_FILE_CLOSE, CONTEXT_MENU_CLOSE},
   {0, NULL},
   {IDM_EDIT_CUT, CONTEXT_MENU_CUT},
   {IDM_EDIT_COPY, CONTEXT_MENU_COPY},
   {IDM_EDIT_PASTE, CONTEXT_MENU_PASTE},
   {0, NULL},
   {IDM_DEBUG_BREAKPOINT, CONTEXT_MENU_TOGGLE},
   {IDM_DEBUG_RUNTO, CONTEXT_MENU_RUNTO},
};

// --- MessageLog ---

class MessageLog : public ListView
{
   _ELENA_::Map<int, MessageBookmark*> _bookmarks;

public:
   MessageBookmark* getBookmark(int index) { return _bookmarks.get(index); }

   void addMessage(text_t message, text_t file, text_t row, text_t col)
   {
      MessageBookmark* bookmark = new MessageBookmark(file, col, row);

      int index = addItem(message);
      setItemText(file, index, 1);
      setItemText(row, index, 2);
      setItemText(col, index, 3);

      _bookmarks.add(index, bookmark);
   }

   virtual void clear()
   {
      ListView::clear();

      _bookmarks.clear();
   }

   MessageLog(Control* owner)
      : ListView(owner), _bookmarks(NULL, _ELENA_::freeobj)
   {
      addLAColumn(_T("Description"), 0, 600);
      addLAColumn(_T("File"), 1, 100);
      addLAColumn(_T("Line"), 2, 100);
      addLAColumn(_T("Column"), 3, 100);
   }
};

// --- CallStackLog ---

class CallStackLog : public ListView
{
   _ELENA_::Map<int, MessageBookmark*> _bookmarks;

   class CallStackWatch : public _ELENA_::_DebuggerCallStack
   {
      CallStackLog* _log;

   public:
      virtual void write(_ELENA_::ident_t moduleName, _ELENA_::ident_t className, _ELENA_::ident_t methodName, _ELENA_::ident_t p, int col, int row, size_t address)
      {
         _ELENA_::IdentifierString source(className);
         _ELENA_::Path path;
         _ELENA_::Path::loadPath(path, p);

         if (!_ELENA_::emptystr(methodName))
         {
            source.append('.');
            source.append(methodName);
         }
         source.append(' ');
         source.append('<');
         source.appendHex(address);
         source.append('>');
         
         int index = _log->addItem(_ELENA_::WideString(source));
         _log->setItemText(path, index, 1);

         wchar_t rowStr[10];
         _ELENA_::StringHelper::intToStr(row, rowStr, 10);
         _log->setItemText(rowStr, index, 2);

         MessageBookmark* bookmark = new MessageBookmark(moduleName, path, col, row);
         _log->_bookmarks.add(index, bookmark);
      }

      virtual void write(size_t address)
      {
         _ELENA_::WideString source;
         source.append('<');
         source.appendHex(address);
         source.append('>');

         _log->addItem(source);
      }

      CallStackWatch(CallStackLog* log)
      {
         _log = log;
      }
   };

   CallStackWatch watch;

public:
   MessageBookmark* getBookmark(int index) { return _bookmarks.get(index); }

   virtual void clear()
   {
      ListView :: clear();

      _bookmarks.clear();
   }

   void refresh(_ELENA_::_DebugController* controller)
   {
      if (isVisible()) {
         CallStackWatch watch(this);
      
         clear();
         controller->readCallStack(&watch);
      }
   }

   CallStackLog(Control* owner)
      : ListView(owner), _bookmarks(NULL, _ELENA_::freeobj), watch(this)
   {
      addLAColumn(_T("Method"), 0, 600);
      addLAColumn(_T("File"), 1, 100);
      addLAColumn(_T("Line"), 2, 100);
   }
};

MenuInfo browserContextMenuInfo[3] = {
	   {IDM_DEBUG_INSPECT, CONTEXT_MENU_INSPECT},
	   {0, NULL},
	   {IDM_DEBUG_SWITCHHEXVIEW, CONTEXT_MENU_SHOWHEX}
};

// --- setForegroundWindow() ---
inline void setForegroundWindow(HWND hwnd)
{
   DWORD dwTimeoutMS;
   // Get the current lock timeout.
   ::SystemParametersInfo (0x2000, 0, &dwTimeoutMS, 0);

   // Set the lock timeout to zero
   ::SystemParametersInfo (0x2001, 0, 0, 0);

   // Perform the SetForegroundWindow
   ::SetForegroundWindow (hwnd);

   // Set the timeout back
   ::SystemParametersInfo (0x2001, 0, (LPVOID)dwTimeoutMS, 0);   //HWND hCurrWnd;
}

// --- ContextBrowser ---

MainWindow::ContextBrowser :: ContextBrowser(Model* model)
{
   _model = model;
   _treeView = NULL;   
}

void MainWindow::ContextBrowser :: assign(Control* treeView)
{
   _treeView = treeView;
   _menu.create(3, browserContextMenuInfo);

   _watch = new DebugWatch(this);
}

bool MainWindow::ContextBrowser::isHexNumberMode()
{
   return _model->hexNumberMode;
}

TreeViewItem MainWindow::ContextBrowser :: hitTest(short x, short y)
{
   TVHITTESTINFO hit;

   hit.pt.x = x;
   hit.pt.y = y;

   ::ScreenToClient(_treeView->getHandle(), &hit.pt);

   HTREEITEM i = TreeView_HitTest(_treeView->getHandle(), &hit);

   return i;
}

void MainWindow::ContextBrowser :: showContextMenu(HWND owner, short x, short y, Model* model)
{
   Point p(x, y);

   _menu.checkItemById(IDM_DEBUG_SWITCHHEXVIEW, model->hexNumberMode);
   _menu.show(owner, p);
}

bool MainWindow::ContextBrowser :: isExpanded(void* node)
{
   return ((TreeView*)_treeView)->isExpanded((TreeViewItem)node);
}

void MainWindow::ContextBrowser::expand(void* node)
{
   ((TreeView*)_treeView)->expand((TreeViewItem)node);
}

void MainWindow::ContextBrowser :: clear(void* node)
{
   ((TreeView*)_treeView)->clear((TreeViewItem)node);
}

void MainWindow::ContextBrowser :: erase(void* node)
{
   ((TreeView*)_treeView)->erase((TreeViewItem)node);
}

void* MainWindow::ContextBrowser :: newNode(void* parent, _ELENA_::ident_t caption, int param)
{
   return (void*)((TreeView*)_treeView)->insertTo((TreeViewItem)parent, _ELENA_::WideString(caption), param);
}

void MainWindow::ContextBrowser :: reset()
{
   _watch->reset();
}

void MainWindow::ContextBrowser :: refresh(_ELENA_::_DebugController* controller)
{
   _watch->refresh(controller);
}

void MainWindow::ContextBrowser :: getCaption(void* node, _ELENA_::ident_c* caption, size_t length)
{
   _ELENA_::WideString s;
   ((TreeView*)_treeView)->getCaption((TreeViewItem)node, s, 0x100);

   _ELENA_::StringHelper::copy(caption, s, _ELENA_::getlength(s), length);
   caption[length] = 0;
}

void MainWindow::ContextBrowser :: setCaption(void* node, _ELENA_::ident_t caption)
{
   ((TreeView*)_treeView)->setCaption((TreeViewItem)node, _ELENA_::WideString(caption));
}

void MainWindow::ContextBrowser::setParam(void* node, size_t param)
{
   ((TreeView*)_treeView)->setParam((TreeViewItem)node, param);
}

size_t MainWindow::ContextBrowser::getParam(void* node)
{
   return ((TreeView*)_treeView)->getParam((TreeViewItem)node);
}

void* MainWindow::ContextBrowser::getCurrent()
{
   return ((TreeView*)_treeView)->getCurrent();
}

void* MainWindow::ContextBrowser :: findNodeStartingWith(void* node, _ELENA_::ident_t name)
{
   _ELENA_::WideString caption(name);

   text_c itemName[CAPTION_LEN];
   size_t nameLen = _ELENA_::getlength(name);

   TreeView* browser = (TreeView*)_treeView;

   TreeViewItem item = browser->getChild((TreeViewItem)node);
   while (item != NULL) {
   //   size_t itemAddress = _browser->getParam(item);
      browser->getCaption(item, itemName, CAPTION_LEN);

      if ((_ELENA_::getlength(itemName) > nameLen + 1) && _ELENA_::StringHelper::compare(itemName, caption, nameLen)
         && itemName[nameLen] == ' ')
      {
         return item;
      }
      item = browser->getNext(item);
   }

   return NULL;
}

void MainWindow::ContextBrowser :: browse(_ELENA_::_DebugController* controller)
{
   browse(controller, getCurrent());
   expand(getCurrent());
}

void MainWindow::ContextBrowser::browse(_ELENA_::_DebugController* controller, void* current)
{
   if (current) {
      size_t address = getParam(current);
      DebuggerWatch subWatch(this, current, address, 0);

      subWatch.refresh(controller);
   }
}

// --- IDEWindow ---

void MainWindow :: _onNotify(NMHDR* notification)
{
   switch (notification->code)
   {
      case TCN_SELCHANGE:
         onClientChanged(TABCHANGED_NOTIFY, notification);
         break;
      case TEXT_VIEW_CHANGED:
         onClientChanged(VIEWCHANGED_NOTIFY, notification);
         break;
      case CONTEXT_MENU_ON:
         onContextMenu((ContextMenuNMHDR*)notification);
         break;
       case TEXT_VIEW_MARGINCLICKED:
          _controller->toggleBreakpoint();
         break;
//      case IDE_DEBUGGER_LOADMODULE:
//         _ide->loadModule(((Message3NMHDR*)notification)->message, ((Message3NMHDR*)notification)->param);
//         break;
//      case IDE_DEBUGGER_LOADTEMPMODULE:
//         _ide->loadTemporalModule(((MessageNMHDR*)notification)->message, ((MessageNMHDR*)notification)->param);
//         break;
      case IDE_DEBUGGER_START:
         _controller->onDebuggerStart();
         break;
      case IDE_DEBUGGER_STEP:
         onDebuggerStep((LineInfoNMHDR*)notification);
         break;
      case IDE_DEBUGGER_STOP:
         onDebuggerStop(false);
         break;
      case IDE_DEBUGGER_BREAK:
         onDebuggerStop(true);
         break;
      case IDE_DEBUGGER_CHECKPOINT:
         _controller->onDebuggerCheckPoint(((MessageNMHDR*)notification)->message);
         break;
      case IDE_DEBUGGER_HOOK:
         _controller->onDebuggerVMHook();
         break;
      case IDM_DEBUGGER_EXCEPTION:
      {
         _ELENA_::String<wchar_t, 255> message(_T("Exception "));
         message.appendHex(((Message2NMHDR*)notification)->param2);
         message.append(_T(": "));
         message.append(((Message2NMHDR*)notification)->message);
         message.appendHex(((Message2NMHDR*)notification)->param1);
         ::MessageBox(getHandle(), message, APP_NAME, MB_OK | MB_ICONERROR); // !!
         break;
      }
      case TVN_SELCHANGED:
         onIndexChange(notification);
         break;
      case NM_DBLCLK:
         onDoubleClick(notification);
         break;
      case TTN_GETDISPINFO:
         if (isTabToolTip(notification->hwndFrom)) {
            _onTabTip((NMTTDISPINFO*)notification);
         }
         else _onToolTip((NMTTDISPINFO*)notification);
         break;
      case IDM_COMPILER_SUCCESSFUL:
         _controller->onCompilationEnd(SUCCESSFULLY_COMPILED, true);

         if (((ExtNMHDR*)notification)->extParam) {
            // repeat menu command after successful compilation
            _onMenuCommand(((ExtNMHDR*)notification)->extParam);
            _controller->onAutoCompilationEnd();
         }
         break;
      case IDM_COMPILER_UNSUCCESSFUL:
         _controller->cleanUpProject();
         displayErrors();
         _controller->onCompilationEnd(COMPILED_WITH_ERRORS, false);
         break;
      case IDM_COMPILER_WITHWARNING:
         displayErrors();
         _controller->onCompilationEnd(COMPILED_WITH_WARNINGS, true);
         break;
      case NM_RCLICK :
         onRClick(notification);
         break;
      case TVN_KEYDOWN:
         _onChildKeyDown(notification);
         break;
      case IDM_LAYOUT_CHANGED:
         onResize();
         break;
      //case IDE_EDITOR_ROWCOUNT_CHANGED:
      //   onRowCountChanged((ExtNMHDR2*)notification);
      //   break;
      case TVN_ITEMEXPANDING:
         onTVItemExpanded((NMTREEVIEW*)notification);
         break;
   }
}

bool MainWindow :: isTabToolTip(HWND handle)
{
   if (!_tabTTHandle)
      _tabTTHandle = ((EditFrame*)_controls[CTRL_EDITFRAME])->getToolTipHandle();

   return _tabTTHandle == handle;
}

void MainWindow :: _onToolTip(NMTTDISPINFO* toolTip)
{
   switch (toolTip->hdr.idFrom) {
      case IDM_FILE_NEW:
         toolTip->lpszText = HINT_NEW_FILE;
         break;
      case IDM_FILE_OPEN:
         toolTip->lpszText = HINT_OPEN_FILE;
         break;
      case IDM_FILE_SAVE:
         toolTip->lpszText = HINT_SAVE_FILE;
         break;
      case IDM_FILE_SAVEALL:
         toolTip->lpszText = HINT_SAVEALL;
         break;
      case IDM_FILE_CLOSE:
         toolTip->lpszText = HINT_CLOSE_FILE;
         break;
      case IDM_PROJECT_CLOSE:
         toolTip->lpszText = HINT_CLOSE_PROJECT;
         break;
      case IDM_EDIT_CUT:
         toolTip->lpszText = HINT_CUT;
         break;
      case IDM_EDIT_COPY:
         toolTip->lpszText = HINT_COPY;
         break;
      case IDM_EDIT_PASTE:
         toolTip->lpszText = HINT_PASTE;
         break;
      case IDM_EDIT_UNDO:
         toolTip->lpszText = HINT_UNDO;
         break;
      case IDM_EDIT_REDO:
         toolTip->lpszText = HINT_REDO;
         break;
      case IDM_DEBUG_RUN:
         toolTip->lpszText = HINT_RUN;
         break;
      case IDM_DEBUG_STOP:
         toolTip->lpszText = HINT_STOP;
         break;
      case IDM_DEBUG_STEPINTO:
         toolTip->lpszText = HINT_STEPINTO;
         break;
	  case IDM_DEBUG_STEPOVER:
         toolTip->lpszText = HINT_STEPOVER;
         break;
	  case IDM_DEBUG_GOTOSOURCE:
         toolTip->lpszText = HINT_GOTOSOURCE;
         break;
      default:
         return;
   }
}

void MainWindow :: _onTabTip(NMTTDISPINFO* tip)
{
   tip->lpszText = (LPWSTR)_model->getDocumentPath(tip->hdr.idFrom);
}

void MainWindow :: _onMenuCommand(int optionID)
{
   switch (optionID) {
      case IDM_PROJECT_NEW:
         _controller->doCreateProject();
         break;
      case IDM_FILE_NEW:
         _controller->doCreateFile();
         break;
      case IDM_FILE_OPEN:
         _controller->doOpenFile();
         break;
      case IDM_PROJECT_OPEN:
         _controller->doOpenProject();
         break;
      case IDM_FILE_CLOSE:
         _controller->doCloseFile();         
         break;
      case IDM_FILE_CLOSEALL:
         _controller->doCloseAll(false);
         break;
      case IDM_FILE_CLOSEALLBUT:
         _controller->doCloseAllButActive();
         break;
      case IDM_FILE_SAVE:
         _controller->doSave(false);
         break;
      case IDM_FILE_SAVEAS:
         _controller->doSave(true);
         break;
      case IDM_FILE_SAVEPROJECT:
         _controller->doSaveProject(true);
         break;
      case IDM_FILE_SAVEALL:
         _controller->doSaveAll(true);
         break;
      case IDM_FILE_EXIT:
         _controller->doExit();
         break;
      case IDM_PROJECT_CLOSE:
         _controller->doCloseAll(true);
         break;
      case IDM_FILE_FILES_1:
      case IDM_FILE_FILES_2:
      case IDM_FILE_FILES_3:
      case IDM_FILE_FILES_4:
      case IDM_FILE_FILES_5:
      case IDM_FILE_FILES_6:
      case IDM_FILE_FILES_7:
      case IDM_FILE_FILES_8:
      case IDM_FILE_FILES_9:
      case IDM_FILE_FILES_10:
         _controller->doOpenFile(_recentFiles.get(optionID));
         break;
      case IDM_FILE_FILES_CLEAR:
         _recentFiles.clear();
         break;
      case IDM_FILE_PROJECTS_1:
      case IDM_FILE_PROJECTS_2:
      case IDM_FILE_PROJECTS_3:
      case IDM_FILE_PROJECTS_4:
      case IDM_FILE_PROJECTS_5:
      case IDM_FILE_PROJECTS_6:
      case IDM_FILE_PROJECTS_7:
      case IDM_FILE_PROJECTS_8:
      case IDM_FILE_PROJECTS_9:
      case IDM_FILE_PROJECTS_10:
         _controller->doOpenProject(_recentProjects.get(optionID));
         break;
      case IDM_FILE_PROJECTS_CLEAR:
         _recentProjects.clear();
         _model->defaultProject.clear();
         break;
      case IDM_EDIT_UNDO:
         _controller->doUndo();
         break;
      case IDM_EDIT_REDO:
         _controller->doRedo();
         break;
      case IDM_EDIT_CUT:
         if (_controller->doEditCopy())
            _controller->doEditDelete();
         break;
      case IDM_EDIT_COPY:
         _controller->doEditCopy();
         break;
      case IDM_EDIT_PASTE:
         _controller->doEditPaste();
         break;
      case IDM_EDIT_DELETE:
         _controller->doEditDelete();
         break;
      case IDM_EDIT_INDENT:
         _controller->doIndent();
         break;
      case IDM_EDIT_OUTDENT:
         _controller->doOutdent();
         break;
      case IDM_EDIT_SELECTALL:
         _controller->doSelectAll();
         break;
      case IDM_EDIT_TRIM:
         _controller->doTrim();
         break;
      case IDM_EDIT_DUPLICATE:
         _controller->doDuplicateLine();
         break;
      case IDM_EDIT_ERASELINE:
         _controller->doEraseLine();
         break;
      case IDM_EDIT_COMMENT:
         _controller->doComment();
         break;
      case IDM_EDIT_UNCOMMENT:
         _controller->doUnComment();
         break;
      case IDM_EDIT_UPPERCASE:
         _controller->doUpperCase();
         break;
      case IDM_EDIT_LOWERCASE:
         _controller->doLowerCase();
         break;
      case IDM_EDIT_SWAP:
         _controller->doSwap();
         break;
      case IDM_VIEW_PROJECTVIEW:
         _controller->doShowProjectView(!_model->projectView);
         break;
      case IDM_VIEW_OUTPUT:
         _controller->doShowCompilerOutput(!_model->compilerOutput);
         break;
      case IDM_VIEW_MESSAGES:
         _controller->doShowMessages(!_model->messages);
         break;
      case IDM_VIEW_WATCH:
         _controller->doShowDebugWatch(!isControlVisible(CTRL_CONTEXTBOWSER));
         break;
      case IDM_VIEW_CALLSTACK:
         _controller->doShowCallStack(!_model->callStack);
         break;
      case IDM_SEARCH_FIND:
         _controller->doFind();
         break;
      case IDM_SEARCH_FINDNEXT:
         _controller->doFindNext();
         break;;
      case IDM_SEARCH_REPLACE:
         _controller->doReplace();
         break;
      case IDM_SEARCH_GOTOLINE:
         _controller->doGoToLine();
         break;
      case IDM_WINDOW_NEXT:
         _controller->doSwitchTab(true);
         break;
      case IDM_WINDOW_PREVIOUS:
         _controller->doSwitchTab(false);
         break;
      case IDM_WINDOW_WINDOWS:
         _controller->doSelectWindow();
         break;
      case IDM_WINDOW_FIRST:
      case IDM_WINDOW_SECOND:
      case IDM_WINDOW_THIRD:
      case IDM_WINDOW_FOURTH:
      case IDM_WINDOW_FIFTH:
      case IDM_WINDOW_SIXTH:
      case IDM_WINDOW_SEVENTH:
      case IDM_WINDOW_EIGHTH:
      case IDM_WINDOW_NINTH:
      case IDM_WINDOW_TENTH:
         _controller->doSelectWindow(_windowList.get(optionID));
         break;
      case IDM_EDITOR_OPTIONS:
         _controller->doSetEditorSettings();
         break;
      case IDM_DEBUGGER_OPTIONS:
         _controller->doSetDebuggerSettings();
         break;
      case IDM_PROJECT_COMPILE:
         _controller->doCompileProject();
         break;
      case IDM_PROJECT_CLEAN:
         _controller->cleanUpProject();
         break;
      case IDM_DEBUG_RUN:
         _controller->doDebugRun();
         break;
      case IDM_DEBUG_RUNTO:
         _controller->doDebugRunTo();
         break;
      case IDM_DEBUG_STEPOVER:
         _controller->doStepOver();
         break;
      case IDM_DEBUG_NEXTSTATEMENT:
         //_controller->doNextStatement();
         _controller->doStepOver();
         break;
      case IDM_DEBUG_STEPINTO:
         _controller->doStepInto();
         break;
      case IDM_DEBUG_STOP:
         _controller->doDebugStop();
         break;
      case IDM_DEBUG_INSPECT:
         _controller->doDebugInspect();
         break;
      case IDM_DEBUG_SWITCHHEXVIEW:
         _controller->doDebugSwitchHexMode();
         break;
      case IDM_DEBUG_BREAKPOINT:
         _controller->toggleBreakpoint();
         break;
      case IDM_DEBUG_CLEARBREAKPOINT:
         _controller->clearBreakpoints();
         break;
      case IDM_DEBUG_GOTOSOURCE:
         _controller->doGotoSource();
         break;
      case IDM_PROJECT_OPTION:
         _controller->doSetProjectSettings();
         break;
      case IDM_PROJECT_FORWARDS:
         _controller->doSetProjectForwards();
         break;
      case IDM_PROJECT_INCLUDE:
         _controller->doInclude();
         break;
      case IDM_PROJECT_EXCLUDE:
         _controller->doExclude();
         break;
      case IDM_HELP_API:
         openHelp();
         break;
      case IDM_HELP_ABOUT:
         _controller->doShowAbout();
         break;
   }
}

void MainWindow :: onClientChanged(int code, NMHDR* notification)
{
   switch(code) {
      case TABCHANGED_NOTIFY:
         onTabChanged(notification->hwndFrom, -1);
         break;
      case VIEWCHANGED_NOTIFY:
         _controller->onCursorChange();
         _controller->onFrameChange();
         break;
   }
}

bool MainWindow :: checkControlHandle(int index, HWND wnd)
{
   return _controls[index]->checkHandle(wnd);
}

bool MainWindow :: isControlVisible(int index)
{
   return ((Control*)_controls[index])->isVisible();
}

void MainWindow :: onTabChanged(HWND wnd, int index)
{
   if (checkControlHandle(CTRL_EDITFRAME, wnd)) {
      EditFrame* mainFrame = (EditFrame*)_controls[CTRL_EDITFRAME];
      mainFrame->onTabChange(index);

      _windowList.add(_model->getDocumentPath(index));

      mainFrame->setFocus();
      _controller->onFrameChange();
      _controller->onDocIncluded();
   }
   else if (checkControlHandle(CTRL_TABBAR, wnd)) {
      ((TabBar*)_controls[CTRL_TABBAR])->onTabChange(index);

      // HOT FIX : refresh call stack
      if (isControlVisible(CTRL_CALLLIST)) {
         _controller->refreshDebuggerInfo();
      }

      _controller->onFrameChange();
   }
}

void MainWindow :: onContextMenu(ContextMenuNMHDR* notification)
{
   bool selected = _model->currentDoc ? _model->currentDoc->hasSelection() : false;

   ((EditFrame*)_controls[CTRL_EDITFRAME])->showContextMenu(notification->x, notification->y, selected);
}

void MainWindow :: _onChildKeyDown(NMHDR* notification)
{
   if (checkControlHandle(CTRL_CONTEXTBOWSER, notification->hwndFrom)) {
      switch (((LPNMLVKEYDOWN)notification)->wVKey) {
         case 73:
            _onMenuCommand(IDM_DEBUG_INSPECT); // !! check if ctrl was pressed too
            break;
      }
   }
}

void MainWindow :: _onDrawItem(DRAWITEMSTRUCT* item)
{
   if (checkControlHandle(CTRL_EDITFRAME, item->hwndItem)) {
      ((EditFrame*)_controls[CTRL_EDITFRAME])->_onDrawItem((DRAWITEMSTRUCT*)item);
   }
   else if (checkControlHandle(CTRL_TABBAR, item->hwndItem)) {
      ((EditFrame*)_controls[CTRL_TABBAR])->_onDrawItem((DRAWITEMSTRUCT*)item);
   }
}

void MainWindow :: onTVItemExpanded(NMTREEVIEW* notification)
{
   if (checkControlHandle(CTRL_CONTEXTBOWSER, notification->hdr.hwndFrom)) {
      _controller->doBrowseWatch((void*)notification->itemNew.hItem);
   }
}

void MainWindow :: browseWatch(_ELENA_::_DebugController* debugController, void* watchNode)
{
   _contextBrowser.browse(debugController, watchNode);
}

void MainWindow::browseWatch(_ELENA_::_DebugController* debugController)
{
   _contextBrowser.browse(debugController);
}

void MainWindow :: onActivate()
{
   ((EditFrame*)_controls[CTRL_EDITFRAME])->setFocus();
}

void MainWindow :: onDoubleClick(NMHDR* notification)
{
   if (checkControlHandle(CTRL_MESSAGELIST, notification->hwndFrom)) {
      _controller->highlightMessage(((MessageLog*)_controls[CTRL_MESSAGELIST])->getBookmark(((LPNMITEMACTIVATE)notification)->iItem), STYLE_ERROR_LINE);
   }
   else if (checkControlHandle(CTRL_CALLLIST, notification->hwndFrom)) {
      _controller->highlightMessage(((CallStackLog*)_controls[CTRL_CALLLIST])->getBookmark(((LPNMITEMACTIVATE)notification)->iItem), STYLE_TRACE_LINE);
   }
}

void MainWindow :: onIndexChange(NMHDR* notification)
{
   if (checkControlHandle(CTRL_PROJECTVIEW, notification->hwndFrom)) {
      TreeView* tree = (TreeView*)_controls[CTRL_PROJECTVIEW];

      int index = tree->getParam(tree->getCurrent());
      if (index != -1) {
         _controller->selectProjectFile(index);
      }
   }
}

void MainWindow :: onRClick(NMHDR* notification)
{
   if (checkControlHandle(CTRL_CONTEXTBOWSER, notification->hwndFrom)) {
      DWORD dwpos = ::GetMessagePos();

      HTREEITEM item = _contextBrowser.hitTest(LOWORD(dwpos), HIWORD(dwpos));
      if (item) {
         ((TreeView*)_controls[CTRL_CONTEXTBOWSER])->select(item);
      }
      _contextBrowser.showContextMenu(_handle, LOWORD(dwpos), HIWORD(dwpos), _model);
   }
}

bool MainWindow :: onClose()
{
   if (!_controller->onClose())
      return false;

   return Window::onClose();
}

void MainWindow :: displayErrors()
{
   _ELENA_::String<wchar_t, 266> message, file;
   _ELENA_::String<wchar_t, 15> colStr, rowStr;

   wchar_t* buffer = ((Output*)_controls[CTRL_OUTPUT])->getOutput();

   const wchar_t* s = buffer;
   while (s) {
      const wchar_t* err = wcsstr(s, _T(": error "));
      if (err==NULL) {
         err = wcsstr(s, _T(": warning "));
      }
      if (err==NULL)
         break;

      const wchar_t* line = err - 1;
      const wchar_t* row = NULL;
      const wchar_t* col = NULL;
      while (true) {
         if (*line=='(') {
            row = line + 1;
         }
         else if (*line==':' && col == NULL) {
            col = line + 1;
         }
         else if (*line == '\n')
            break;

         line--;
      }
      s = wcschr(err, '\n');

      message.copy(err + 2, s - err- 3);
      if (row==NULL) {
         file.clear();
         colStr.clear();
         rowStr.clear();
      }
      else {
         file.copy(line + 1, row - line - 2);
         if (col != NULL) {
            rowStr.copy(row, col - row - 1);
            colStr.copy(col, err - col - 1);
         }
         else {
            rowStr.copy(row, err - row - 1);
            colStr.clear();
         }
      }
      ((MessageLog*)_controls[CTRL_MESSAGELIST])->addMessage(message, file, rowStr, colStr);

      //break;
   }
   _controller->doShowCompilerOutput(true);
   if (_model->messages) {
      ((TabBar*)_controls[CTRL_TABBAR])->selectTabChild((Control*)_controls[CTRL_MESSAGELIST]);
   }
   else _controller->doShowMessages(true);

   _ELENA_::freestr(buffer);
}

bool MainWindow :: copyToClipboard(Document* document)
{
   if (_clipboard.begin(this)) {
      _clipboard.clear();
      
      HGLOBAL buffer = _clipboard.create(document->getSelectionLength());
      wchar_t* text = _clipboard.allocate(buffer);
      
      document->copySelection(text);
      
      _clipboard.free(buffer);
      _clipboard.copy(buffer);
      
      _clipboard.end();

      return true;
   }
   else return false;
}

void MainWindow :: pasteFrameClipboard(Document* document)
{
   if (_clipboard.begin(this)) {
      HGLOBAL buffer = _clipboard.get();
      wchar_t* text = _clipboard.allocate(buffer);
      if  (!_ELENA_::emptystr(text)) {
         document->insertLine(text, _ELENA_::getlength(text));
      
         _clipboard.free(buffer);
      }

      _clipboard.end();
   }
}

int MainWindow :: getCurrentDocumentIndex()
{
   return ((EditFrame*)_controls[CTRL_EDITFRAME])->getCurrentDocumentIndex();
}

int MainWindow :: newDocument(text_t name, Document* doc)
{
   return ((EditFrame*)_controls[CTRL_EDITFRAME])->newDocument(name, doc);
}

MainWindow :: MainWindow(HINSTANCE instance, const wchar_t* caption, _Controller* controller, Model* model)
   : SDIWindow(instance, caption), _windowList(10, IDM_WINDOW_WINDOWS), _recentFiles(10, IDM_FILE_FILES), _recentProjects(10, IDM_FILE_PROJECTS), _contextBrowser(model)
{
   _controller = controller;
   _model = model;
   _tabTTHandle = NULL;

   _controlCount = 14;
   _controls = (_BaseControl**)malloc(_controlCount << 2);

   _controls[0] = NULL;
   _controls[CTRL_MENU] = new Menu(instance, IDR_IDE_ACCELERATORS, ::GetMenu(getHandle()));
   _controls[CTRL_CONTEXTMENU] = new ContextMenu();

   _windowList.assign((Menu*)_controls[CTRL_MENU]);
   _recentFiles.assign((Menu*)_controls[CTRL_MENU]);
   _recentProjects.assign((Menu*)_controls[CTRL_MENU]);

   ContextMenu* contextMenu = (ContextMenu*)_controls[CTRL_CONTEXTMENU];

   contextMenu->create(8, contextMenuInfo);

   _controls[CTRL_STATUSBAR] = new StatusBar(this, 5, StatusBarWidths);
   _controls[CTRL_TOOLBAR] = new ToolBar(this, 16, AppToolBarButtonNumber, AppToolBarButtons);
   _controls[CTRL_EDITFRAME] = new EditFrame(this, true, contextMenu, model);
   _controls[CTRL_TABBAR] = new TabBar(this, _model->tabWithAboveScore);
   _controls[CTRL_OUTPUT] = new Output((Control*)_controls[CTRL_TABBAR], this);
   _controls[CTRL_MESSAGELIST] = new MessageLog((Control*)_controls[CTRL_TABBAR]);
   _controls[CTRL_CALLLIST] = new CallStackLog((Control*)_controls[CTRL_TABBAR]);
   _controls[CTRL_BSPLITTER] = new Splitter(this, (Control*)_controls[CTRL_TABBAR], false, IDM_LAYOUT_CHANGED);
   _controls[CTRL_CONTEXTBOWSER] = new TreeView((Control*)_controls[CTRL_TABBAR], true);
   _controls[CTRL_PROJECTVIEW] = new TreeView(this, false);
   _controls[CTRL_HSPLITTER] = new Splitter(this, (Control*)_controls[CTRL_PROJECTVIEW], true, IDM_LAYOUT_CHANGED);

   ((Control*)_controls[CTRL_TABBAR])->_setHeight(120);
   ((Control*)_controls[CTRL_BSPLITTER])->_setConstraint(60, 100);
   ((Control*)_controls[CTRL_PROJECTVIEW])->_setWidth(200);

   _statusBar = (StatusBar*)_controls[CTRL_STATUSBAR];

   EditFrame* frame = (EditFrame*)_controls[CTRL_EDITFRAME];
   TextView* textView = new TextView(frame, 5, 28, 400, 400);
   frame->populate(textView);
   textView->setReceptor(this);
   _contextBrowser.assign((Control*)_controls[CTRL_CONTEXTBOWSER]);
   
   setLeft(CTRL_HSPLITTER);
   setTop(CTRL_TOOLBAR);
   setClient(CTRL_EDITFRAME);
   setBottom(CTRL_BSPLITTER);

   frame->init(model);

   showControls(CTRL_STATUSBAR, CTRL_EDITFRAME);

   showControls(CTRL_PROJECTVIEW, CTRL_PROJECTVIEW);
}

MainWindow :: ~MainWindow()
{
   for (size_t i = 0; i < _controlCount; i++)
      _ELENA_::freeobj(_controls[i]);

   _ELENA_::freeobj(_controls);
}

void MainWindow :: setTop(int index)
{
   _layoutManager.setAsTop((Control*)_controls[index]);
}

void MainWindow :: setLeft(int index)
{
   _layoutManager.setAsLeft((Control*)_controls[index]);
}

void MainWindow :: setRight(int index)
{
   _layoutManager.setAsRight((Control*)_controls[index]);
}

void MainWindow::setBottom(int index)
{
   _layoutManager.setAsBottom((Control*)_controls[index]);
}

void MainWindow::setClient(int index)
{
   _layoutManager.setAsClient((Control*)_controls[index]);
}

void MainWindow :: showControls(int from, int till)
{
   for (int i = from ; i <= till ; i++) {
      ((Control*)_controls[i])->show();
   }
}

void MainWindow :: hideControl(int index)
{
   ((Control*)_controls[index])->hide();
}

void MainWindow :: refreshControl(int index)
{
   ((Control*)_controls[index])->refresh();
}

Menu* MainWindow :: getMenu()
{
   return (Menu*)_controls[CTRL_MENU];
}

ToolBar* MainWindow :: getToolBar()
{
   return (ToolBar*)_controls[CTRL_TOOLBAR];
}

void MainWindow :: showFrame()
{
   EditFrame* frame = (EditFrame*)_controls[CTRL_EDITFRAME];

   frame->show();
   frame->setFocus();
}

void MainWindow :: activateFrame()
{
   ((EditFrame*)_controls[CTRL_EDITFRAME])->setFocus();
}

void MainWindow :: hideFrame()
{
   EditFrame* frame = (EditFrame*)_controls[CTRL_EDITFRAME];

   frame->hide();
}

void MainWindow :: selectDocument(int docIndex)
{
   ((EditFrame*)_controls[CTRL_EDITFRAME])->setFocus();

   ((EditFrame*)_controls[CTRL_EDITFRAME])->selectTab(docIndex);
}

void MainWindow :: closeDocument(int docIndex)
{
   ((EditFrame*)_controls[CTRL_EDITFRAME])->eraseDocumentTab(docIndex);
}

void MainWindow :: markDocumentTitle(int docIndex, bool changed)
{
   ((EditFrame*)_controls[CTRL_EDITFRAME])->markDocument(docIndex, changed);
}

void MainWindow :: refreshDocument()
{
   ((EditFrame*)_controls[CTRL_EDITFRAME])->refreshDocument();
}

void MainWindow :: setStatusBarText(int index, text_t message)
{
   ((StatusBar*)_statusBar)->setText(index, message);
}

void MainWindow :: renameDocument(int index, text_t name)
{
   ((EditFrame*)_controls[CTRL_EDITFRAME])->renameDocumentTab(index, name);
}

void MainWindow :: addToWindowList(const wchar_t* path)
{
   _windowList.add(path);
}

void MainWindow::removeFromWindowList(const wchar_t* path)
{
   _windowList.remove(path);
}

void MainWindow :: addToRecentFileList(const wchar_t* path)
{
   _recentFiles.add(path);
}

void MainWindow :: addToRecentProjectList(const wchar_t* path)
{
   _recentProjects.add(path);
}

void MainWindow :: reloadSettings()
{
   ((EditFrame*)_controls[CTRL_EDITFRAME])->init(_model);
}

void MainWindow :: openHelp()
{
   _ELENA_::Path apiPath(_model->paths.appPath);
   apiPath.combine(_T("..\\doc\\api\\index.html"));

   ShellExecute(NULL, _T("open"), apiPath, NULL, NULL, SW_SHOW);
}

void MainWindow :: openOutput()
{
   TabBar* tabBar = ((TabBar*)_controls[CTRL_TABBAR]);

   tabBar->addTabChild(OUTPUT_TAB, (Control*)_controls[CTRL_OUTPUT]);
   tabBar->selectTabChild((Control*)_controls[CTRL_OUTPUT]);
   tabBar->show();
}

void MainWindow::switchToOutput()
{
   ((TabBar*)_controls[CTRL_TABBAR])->selectTabChild((Control*)_controls[CTRL_OUTPUT]);
}

void MainWindow :: clearMessageList()
{
   ((MessageLog*)_controls[CTRL_MESSAGELIST])->clear();
}

void MainWindow :: closeOutput()
{
   TabBar* tabBar = ((TabBar*)_controls[CTRL_TABBAR]);

   tabBar->removeTabChild((Control*)_controls[CTRL_OUTPUT]);
   if (tabBar->getTabCount() == 0) {
      tabBar->hide();
   }
}

void MainWindow :: openMessageList()
{
   TabBar* tabBar = ((TabBar*)_controls[CTRL_TABBAR]);

   tabBar->addTabChild(MESSAGES_TAB, (Control*)_controls[CTRL_MESSAGELIST]);
   tabBar->show();
   tabBar->selectTabChild((Control*)_controls[CTRL_MESSAGELIST]);
}

void MainWindow :: closeMessageList()
{
   TabBar* tabBar = ((TabBar*)_controls[CTRL_TABBAR]);

   tabBar->removeTabChild((Control*)_controls[CTRL_MESSAGELIST]);

   if (tabBar->getTabCount() == 0) {
      tabBar->hide();
   }
}

void MainWindow :: openCallList()
{
   TabBar* tabBar = ((TabBar*)_controls[CTRL_TABBAR]);

   tabBar->addTabChild(CALLSTACK_TAB, (Control*)_controls[CTRL_CALLLIST]);
   tabBar->show();
   tabBar->selectTabChild((Control*)_controls[CTRL_CALLLIST]);
}

void MainWindow::closeCallList()
{
   TabBar* tabBar = ((TabBar*)_controls[CTRL_TABBAR]);

   tabBar->removeTabChild((Control*)_controls[CTRL_CALLLIST]);

   if (tabBar->getTabCount() == 0) {
      tabBar->hide();
   }
}

void MainWindow :: openProjectView()
{
   showControls(CTRL_PROJECTVIEW, CTRL_PROJECTVIEW);
}

void MainWindow :: closeProjectView()
{
   hideControl(CTRL_PROJECTVIEW);
}

bool MainWindow :: compileProject(_ProjectManager* project, int postponedAction)
{
   _ELENA_::Path path(_model->project.path);
   path.combine(_model->project.name);
   path.appendExtension(_T("prj"));

   _ELENA_::Path appPath(_model->paths.appPath);
   appPath.combine(_T("elc.exe"));

   _ELENA_::Path curDir(path, _ELENA_::StringHelper::findLast(path, '\\'));

   _ELENA_::Path cmdLine(_T("elc.exe"));

   const char* options = project->getOptions();
   if (!_ELENA_::emptystr(options)) {
      cmdLine.append(' ');
      _ELENA_::Path::appendPath(cmdLine, options);
   }

   if (_ELENA_::StringHelper::find(path, _T(' '))) {
      cmdLine.append(_T(" \""));
      cmdLine.append(_T("-c"));
      cmdLine.append(path);
      cmdLine.append(_T("\""));
   }
   else {
      cmdLine.append(_T(" -c"));
      cmdLine.append(path);
   }

   //!! HOT FIX to deal with variable tab size
   cmdLine.append(_T(" -xtab"));
   cmdLine.appendInt(_model->tabSize);

   return ((Output*)_controls[CTRL_OUTPUT])->execute(appPath, cmdLine, curDir, postponedAction);
}

void MainWindow :: _notify(int code)
{
   NMHDR notification;

   notification.code = code;
   notification.hwndFrom = NULL;

   ::SendMessage(_handle, WM_NOTIFY, 0, (LPARAM)&notification);
}

void MainWindow :: _notify(int code, const wchar_t* message, int param)
{
   MessageNMHDR notification;

   notification.nmhrd.code = code;
   notification.nmhrd.hwndFrom = NULL;
   notification.message = message;
   notification.param = param;

   ::SendMessage(_handle, WM_NOTIFY, 0, (LPARAM)&notification);
}

void MainWindow :: _notify(int code, const wchar_t* message, const wchar_t* param)
{
   Message3NMHDR notification;

   notification.nmhrd.code = code;
   notification.nmhrd.hwndFrom = NULL;
   notification.message = message;
   notification.param = param;

   ::SendMessage(_handle, WM_NOTIFY, 0, (LPARAM)&notification);
}

void MainWindow :: _notify(int code, const wchar_t* message, int param1, int param2)
{
   Message2NMHDR notification;

   notification.nmhrd.code = code;
   notification.nmhrd.hwndFrom = NULL;
   notification.message = message;
   notification.param1 = param1;
   notification.param2 = param2;

   ::SendMessage(_handle, WM_NOTIFY, 0, (LPARAM)&notification);
}

void MainWindow :: _notify(int code, const wchar_t* ns, const wchar_t* source, HighlightInfo info)
{
   LineInfoNMHDR notification;

   notification.nmhrd.code = code;
   notification.nmhrd.hwndFrom = NULL;
   notification.ns = ns;
   notification.file = source;
   notification.position = info;

   ::SendMessage(_handle, WM_NOTIFY, 0, (LPARAM)&notification);
}

void MainWindow :: resetDebugWindows()
{
   _contextBrowser.reset();
   ((CallStackLog*)_controls[CTRL_CALLLIST])->clear();
}

void MainWindow :: refreshDebugWindows(_ELENA_::_DebugController* debugController)
{
   _contextBrowser.refresh(debugController);
   ((CallStackLog*)_controls[CTRL_CALLLIST])->refresh(debugController);
}

void MainWindow :: openDebugWatch()
{
   if (!isControlVisible(CTRL_CONTEXTBOWSER)) {
      TabBar* tabBar = ((TabBar*)_controls[CTRL_TABBAR]);

      tabBar->addTabChild(WATCH_TAB, (Control*)_controls[CTRL_CONTEXTBOWSER]);
      tabBar->selectTabChild((Control*)_controls[CTRL_CONTEXTBOWSER]);
      tabBar->show();

      showControls(CTRL_CONTEXTBOWSER, CTRL_CONTEXTBOWSER);
   }
   refreshControl(CTRL_CONTEXTBOWSER);
   refresh();
}

void MainWindow :: closeDebugWatch()
{
   TabBar* tabBar = ((TabBar*)_controls[CTRL_TABBAR]);

   tabBar->removeTabChild((Control*)_controls[CTRL_CONTEXTBOWSER]);
   if (tabBar->getTabCount() == 0) {
      tabBar->hide();
   }
   else tabBar->selectLastTabChild();

   hideControl(CTRL_CONTEXTBOWSER);
   refresh();
}

void MainWindow :: onDebuggerStep(LineInfoNMHDR* notification)
{
   setForegroundWindow(_handle);

   if (::IsIconic(_handle))
      ::ShowWindowAsync(_handle, _model->appMaximized ? SW_MAXIMIZE : SW_SHOWNORMAL);

   _controller->onDebuggerStep(notification->ns, notification->file, notification->position);
}

void MainWindow :: onDebuggerStop(bool broken)
{
   setForegroundWindow(_handle);

   if (::IsIconic(_handle))
      ::ShowWindowAsync(_handle, _model->appMaximized ? SW_MAXIMIZE : SW_SHOWNORMAL);

   _controller->onDebuggerStop(broken);
}

void MainWindow :: reloadProjectView(_ProjectManager* project)
{
   TreeView* projectView = (TreeView*)_controls[CTRL_PROJECTVIEW];

   projectView->clear(NULL);

   TreeViewItem root = projectView->insertTo(NULL, _model->project.name, -1);

   _ELENA_::ConfigCategoryIterator it = project->SourceFiles();
   int index = 0;
   _ELENA_::wide_c buffer[0x100];
   while (!it.Eof()) {
      _ELENA_::ident_t name = it.key();

      TreeViewItem parent = root;
      int start = 0;      
      while (true) {
         int end = _ELENA_::StringHelper::find(name + start, PATH_SEPARATOR);

         _ELENA_::WideString nodeName(name + start, (end == -1 ? _ELENA_::getlength(name) : end) - start);

         TreeViewItem current = projectView->getChild(parent);
         while (current != NULL) {
            projectView->getCaption(current, buffer, 0x100);

            if (!nodeName.compare(buffer)) {
               current = projectView->getNext(current);
            }
            else break;
         }

         if (current == NULL) {
            current = projectView->insertTo(parent, nodeName, end == -1 ? index : -1);
         }
         parent = current;

         if (end != -1) {
            start = end + 1;
         }
         else break;
      }

      it++;
      index++;
   }

   projectView->expand(root);
}