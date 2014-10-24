//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//    WinAPI: IDE & IDE Controls
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#include "winide.h"
#include "win32//pehelper.h"

using namespace _GUI_;

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

MenuInfo browserContextMenuInfo[3] = {
	   {IDM_DEBUG_INSPECT, CONTEXT_MENU_INSPECT},
	   {0, NULL},
	   {IDM_DEBUG_SWITCHHEXVIEW, CONTEXT_MENU_SHOWHEX}
};

// --- removeFile ---
inline void removeFile(const wchar_t* name)
{
   _wremove(name);
}

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

// --- Win32ContextBrowser ---

Win32ContextBrowser :: Win32ContextBrowser(Control* owner)
   : ContextBrowser(owner)
{
   _menu.create(3, browserContextMenuInfo);
}

TreeViewItem Win32ContextBrowser :: hitTest(short x, short y)
{
   TVHITTESTINFO hit;

   hit.pt.x = x;
   hit.pt.y = y;

   ::ScreenToClient(_handle, &hit.pt);

   HTREEITEM i = TreeView_HitTest(_handle, &hit);

   return i;
}

void Win32ContextBrowser :: showContextMenu(HWND owner, short x, short y)
{
   Point p(x, y);

   _menu.checkItemById(IDM_DEBUG_SWITCHHEXVIEW, Settings::hexNumberMode);
   _menu.show(owner, p);
}

// --- IDEWindow ---

void MainWindow :: _onNotify(NMHDR* notification)
{
   switch (notification->code)
   {
      case TCN_SELCHANGE:
         _ide->onClientChanged(TABCHANGED_NOTIFY, notification);
         break;
      case TEXT_VIEW_CHANGED:
         _ide->onClientChanged(VIEWCHANGED_NOTIFY, notification);
         break;
      case CONTEXT_MENU_ON:
         _ide->onContextMenu((ContextMenuNMHDR*)notification);
         break;
       case TEXT_VIEW_MARGINCLICKED:
         _ide->toggleBreakpoint();
         break;
      case IDE_DEBUGGER_LOADMODULE:
         _ide->loadModule(((Message3NMHDR*)notification)->message, ((Message3NMHDR*)notification)->param);
         break;
      case IDE_DEBUGGER_LOADTEMPMODULE:
         _ide->loadTemporalModule(((MessageNMHDR*)notification)->message, ((MessageNMHDR*)notification)->param);
         break;
      case IDE_DEBUGGER_START:
         _ide->onDebuggerStart();
         break;
      case IDE_DEBUGGER_STEP:
         _ide->onDebuggerStep(((LineInfoNMHDR*)notification)->ns, ((LineInfoNMHDR*)notification)->file,
                        ((LineInfoNMHDR*)notification)->position);
         break;
      case IDE_DEBUGGER_STOP:
         _ide->onDebuggerStop(false);
         break;
      case IDE_DEBUGGER_BREAK:
         _ide->onDebuggerStop(true);
         break;
      case IDE_DEBUGGER_CHECKPOINT:
         _ide->onDebuggerCheckPoint(((MessageNMHDR*)notification)->message);
         break;
      case IDE_DEBUGGER_HOOK:
         _ide->onDebuggerVMHook();
         break;
      case IDM_DEBUGGER_EXCEPTION:
      {
         _ELENA_::String<tchar_t, 255> message(_T("Exception "));
         message.appendHex(((Message2NMHDR*)notification)->param2);
         message.append(_T(": "));
         message.append(((Message2NMHDR*)notification)->message);
         message.appendHex(((Message2NMHDR*)notification)->param1);
         ::MessageBox(getHandle(), message, APP_NAME, MB_OK | MB_ICONERROR); // !!
         break;
      }
      case NM_DBLCLK:
         _ide->onDoubleClick(notification);
         break;
      case TTN_GETDISPINFO:
         if (isTabToolTip(notification->hwndFrom)) {
            _onTabTip((NMTTDISPINFO*)notification);
         }
         else _onToolTip((NMTTDISPINFO*)notification);
         break;
      case IDM_COMPILER_SUCCESSFUL:
         _ide->onCompilationEnd(SUCCESSFULLY_COMPILED, true);

         if (((ExtNMHDR*)notification)->extParam) {
            // repeat menu command after successful compilation
            _onMenuCommand(((ExtNMHDR*)notification)->extParam);
            _ide->onAutoCompilationEnd();
         }
         break;
      case IDM_COMPILER_UNSUCCESSFUL:
         _ide->cleanUpProject();
         _ide->displayErrors();
         _ide->onCompilationEnd(COMPILED_WITH_ERRORS, false);
         break;
      case IDM_COMPILER_WITHWARNING:
         _ide->displayErrors();
         _ide->onCompilationEnd(COMPILED_WITH_WARNINGS, true);
         break;
      case NM_RCLICK :
         _ide->onRClick(notification);
         break;
      case TVN_KEYDOWN:
         _onChildKeyDown(notification);
         break;
      case IDM_LAYOUT_CHANGED:
         onResize();
         break;
   //   case IDE_EDITOR_ROWCOUNT_CHANGED:
   //      onRowCountChanged((ExtNMHDR2*)notification);
   //      break;
      case TVN_ITEMEXPANDING:
         _ide->onTVItemExpanded((NMTREEVIEW*)notification);
         break;
   }
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
   tip->lpszText = (LPWSTR)_ide->getDocumentPath(tip->hdr.idFrom);
}

void MainWindow :: _onMenuCommand(int optionID)
{
   switch (optionID) {
      case IDM_PROJECT_NEW:
         _ide->doCreateProject();
         break;
      case IDM_FILE_NEW:
         _ide->doCreateFile();
         break;
      case IDM_FILE_OPEN:
         _ide->doOpenFile();
         break;
      case IDM_PROJECT_OPEN:
         _ide->doOpenProject();
         break;
      case IDM_FILE_CLOSE:
         _ide->doCloseFile();
         _ide->onChange();
         break;
      case IDM_FILE_CLOSEALL:
         _ide->closeAll(false);
         _ide->onChange();
         break;
      case IDM_FILE_CLOSEALLBUT:
         _ide->doCloseAllButActive();
         _ide->onChange();
         break;
      case IDM_FILE_SAVE:
         _ide->doSave(false);
         _ide->onChange();
         break;
      case IDM_FILE_SAVEAS:
         _ide->doSave(true);
         _ide->onChange();
         break;
      case IDM_FILE_SAVEPROJECT:
         _ide->doSaveProject(true);
         _ide->onChange();
         break;
      case IDM_FILE_SAVEALL:
         _ide->doSaveAll(true);
         _ide->onChange();
         break;
      case IDM_FILE_EXIT:
         _ide->doExit();
         break;
      case IDM_PROJECT_CLOSE:
         _ide->closeAll();
         _ide->onChange();
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
         _ide->doSelectFile(optionID);
         break;
      case IDM_FILE_FILES_CLEAR:
         _ide->doClearFileHistory();
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
         _ide->doSelectProject(optionID);
         break;
      case IDM_FILE_PROJECTS_CLEAR:
         _ide->doClearProjectHistory();
         break;
      case IDM_EDIT_UNDO:
         _ide->doUndo();
         break;
      case IDM_EDIT_REDO:
         _ide->doRedo();
         break;
      case IDM_EDIT_CUT:
         if (_ide->doEditCopy())
            _ide->doEditDelete();
         _ide->onFrameChange();
         break;
      case IDM_EDIT_COPY:
         _ide->doEditCopy();
         _ide->onFrameChange();
         break;
      case IDM_EDIT_PASTE:
         _ide->doEditPaste();
         break;
      case IDM_EDIT_DELETE:
         _ide->doEditDelete();
         break;
      case IDM_EDIT_INDENT:
         _ide->doIndent();
         break;
      case IDM_EDIT_OUTDENT:
         _ide->doOutdent();
         break;
      case IDM_EDIT_SELECTALL:
         _ide->doSelectAll();
         break;
      case IDM_EDIT_TRIM:
         _ide->doTrim();
         break;
      case IDM_EDIT_DUPLICATE:
         _ide->doDuplicateLine();
         break;
      case IDM_EDIT_ERASELINE:
         _ide->doEraseLine();
         break;
      case IDM_EDIT_COMMENT:
         _ide->doComment();
         break;
      case IDM_EDIT_UNCOMMENT:
         _ide->doUnComment();
         break;
      case IDM_EDIT_UPPERCASE:
         _ide->doUpperCase();
         break;
      case IDM_EDIT_LOWERCASE:
         _ide->doLowerCase();
         break;
      case IDM_EDIT_SWAP:
         _ide->doSwap();
         break;
      case IDM_VIEW_OUTPUT:
         _ide->doShowCompilerOutput(!Settings::compilerOutput);
         break;
      case IDM_VIEW_MESSAGES:
         _ide->doShowMessages(!Settings::messages);
         break;
      case IDM_VIEW_WATCH:
         _ide->doShowDebugWatch();
         break;
      case IDM_VIEW_CALLSTACK:
         _ide->doShowCallStack(!Settings::callStack);
         break;
      case IDM_SEARCH_FIND:
         _ide->doFind();
         break;
      case IDM_SEARCH_FINDNEXT:
         _ide->doFindNext();
         break;;
      case IDM_SEARCH_REPLACE:
         _ide->doReplace();
         break;
      case IDM_SEARCH_GOTOLINE:
         _ide->doGoToLine();
         break;
      case IDM_WINDOW_NEXT:
         _ide->doSwitchTab(true);
         break;
      case IDM_WINDOW_PREVIOUS:
         _ide->doSwitchTab(false);
         break;
      case IDM_WINDOW_WINDOWS:
         _ide->doSelectWindow();
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
         _ide->doSelectWindow(optionID);
         break;
      case IDM_EDITOR_OPTIONS:
         _ide->doSetEditorSettings();
         break;
      case IDM_DEBUGGER_OPTIONS:
         _ide->doSetDebuggerSettings();
         break;
      case IDM_PROJECT_COMPILE:
         _ide->doCompileProject();
         break;
      case IDM_PROJECT_CLEAN:
         _ide->cleanUpProject();
         break;
      case IDM_DEBUG_RUN:
         _ide->doDebugRun();
         break;
      case IDM_DEBUG_RUNTO:
         _ide->doDebugRunTo();
         break;
      case IDM_DEBUG_STEPOVER:
         _ide->doStepOver();
         break;
      case IDM_DEBUG_NEXTSTATEMENT:
         _ide->doNextStatement();
         break;
      case IDM_DEBUG_STEPINTO:
         _ide->doStepInto();
         break;
      case IDM_DEBUG_STOP:
         _ide->doDebugStop();
         break;
      case IDM_DEBUG_INSPECT:
         _ide->doDebugInspect();
         break;
      case IDM_DEBUG_SWITCHHEXVIEW:
         _ide->doDebugSwitchHexMode();
         break;
      case IDM_DEBUG_BREAKPOINT:
         _ide->toggleBreakpoint();
         break;
      case IDM_DEBUG_CLEARBREAKPOINT:
         _ide->clearBreakpoints();
         break;
      case IDM_DEBUG_GOTOSOURCE:
         _ide->doGotoSource();
         break;
      case IDM_PROJECT_OPTION:
         _ide->showProjectSettings();
         break;
      case IDM_PROJECT_FORWARDS:
         _ide->showProjectForwards();
         break;
      case IDM_PROJECT_INCLUDE:
         _ide->doInclude();
         break;
      case IDM_PROJECT_EXCLUDE:
         _ide->doExclude();
         break;
      case IDM_HELP_API:
         _ide->openHelp();
         break;
      case IDM_HELP_ABOUT:
         _ide->doShowAbout();
         break;
   }
}

void MainWindow :: _onChildKeyDown(NMHDR* notification)
{
   if (_ide->isBrowser(notification->hwndFrom)) {
      switch (((LPNMLVKEYDOWN)notification)->wVKey) {
         case 73:
            _onMenuCommand(IDM_DEBUG_INSPECT); // !! check if ctrl was pressed too
            break;
      }
   }
}

void MainWindow :: _onDrawItem(DRAWITEMSTRUCT* item)
{
   _ide->onCustomDraw(item->hwndItem, item);
}

void MainWindow :: onActivate()
{
   _ide->onActivate();
}

bool MainWindow :: onClose()
{
   if (!_ide->onClose())
      return false;

   return Window::onClose();
}

MainWindow :: MainWindow(HINSTANCE instance, const wchar_t* caption, WIN32IDE* env)
   : SDIWindow(instance, caption)
{
   _ide = env;
   _tabTTHandle = NULL;
}

// -- WIN32IDE ---
TabBar* WIN32IDE :: createOutputBar()
{
   TabBar* tabBar = new TabBar(_appWindow, Settings::tabWithAboveScore);

   _output = new Output(tabBar, _appWindow);
   _messageList = new MessageLog(tabBar);
   _callStackList = new CallStackLog(tabBar);
   
   tabBar->_setHeight(120);

   Splitter* bottomSplitter = new Splitter(_appWindow, tabBar, false, IDM_LAYOUT_CHANGED);

   bottomSplitter->_setConstraint(60, 100);

   _appWindow->_getLayoutManager()->setAsBottom(bottomSplitter);

   controls.add(tabBar);
   controls.add(_output);
   controls.add(_messageList);
   controls.add(_callStackList);
   controls.add(bottomSplitter);

   return tabBar;
}

ContextBrowser* WIN32IDE :: createContextBrowser()
{
   Win32ContextBrowser* browser = new Win32ContextBrowser(_appWindow);
   Splitter* leftSplitter = new Splitter(_appWindow, browser, true, IDM_LAYOUT_CHANGED);

   browser->_setWidth(200);

   _appWindow->_getLayoutManager()->setAsLeft(leftSplitter);

   controls.add(browser);
   controls.add(leftSplitter);

   return browser;
}

void WIN32IDE :: start(bool maximized)
{
   onIDEInit();

   _appWindow->show(maximized);

   IDE::start();
}

void WIN32IDE :: onProjectClose()
{
   ((Output*)_output)->clear();

   IDE::onProjectClose();
}

bool WIN32IDE :: compileProject(int postponedAction)
{
   _ELENA_::Path path(Project::getPath(), Project::getName());
   path.appendExtension(_T("prj"));

   _ELENA_::Path appPath(Paths::appPath, _T("elc.exe"));
   _ELENA_::Path curDir(path, _ELENA_::StringHelper::findLast(path, '\\'));

   _ELENA_::Path cmdLine(/*Settings::unicodeELC ? _T("elc.exe -xunicode") : */_T("elc.exe"));

   const char* options = Project::getOptions();
   if (!_ELENA_::emptystr(options)) {
      cmdLine.append(' ');
      cmdLine.append(options);
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
   cmdLine.appendInt(Settings::tabSize);

   return ((Output*)_output)->execute(appPath, cmdLine, curDir, postponedAction);
}

void WIN32IDE :: onCustomDraw(void* handle, void* item)
{
   if (handle == _mainFrame->getHandle()) {
      _mainFrame->_onDrawItem((DRAWITEMSTRUCT*)item);
   }
   else if (handle==_outputBar->getHandle()) {
      _outputBar->_onDrawItem((DRAWITEMSTRUCT*)item);
   }
}

void WIN32IDE :: displayErrors()
{
   _ELENA_::String<wchar_t, 266> message, file;
   _ELENA_::String<wchar_t, 15> colStr, rowStr;

   wchar_t* buffer = ((Output*)_output)->getOutput();

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
      _messageList->addMessage(message, file, rowStr, colStr);

      //break;
   }
   doShowCompilerOutput(true);
   _outputBar->selectTab(1);

   _ELENA_::freestr(buffer);
}

void WIN32IDE :: cleanUpProject()
{
   // clean exe and dm files
   if (!_ELENA_::emptystr(Project::getTarget()))
   {
      _ELENA_::Path targetFile(Project::getPath(), Project::getTarget());
      removeFile(targetFile);

      targetFile.changeExtension(_T("dn"));
      removeFile(targetFile);
   }
   // clean module files


   _ELENA_::ConstantIdentifier ext("nl");
   _ELENA_::ConstantIdentifier d_ext("dnl");
   _ELENA_::Path rootPath(Project::getPath(), Project::getOutputPath());
   for (_ELENA_::ConfigCategoryIterator it = Project::SourceFiles() ; !it.Eof() ; it++) {
      _ELENA_::Path source(rootPath, it.key());

      _ELENA_::Path module;
      module.copyPath(it.key());

      _ELENA_::ReferenceNs name(Project::getPackage());
      name.pathToName(module);          // get a full name

      // remove module
      module.copy(rootPath);
      module.nameToPath(name, ext);
      removeFile(module);

      // remove debug info module
      module.changeExtension(d_ext);
      removeFile(module);
   }
}

void WIN32IDE :: openHelp()
{
   _ELENA_::Path apiPath(Paths::appPath, _T("..\\doc\\api\\index.html"));

   ShellExecute(NULL, _T("open"), apiPath, NULL, NULL, SW_SHOW);
}


void WIN32IDE :: onDebuggerStep(const wchar16_t* ns, const wchar_t* source, HighlightInfo info)
{
   HWND hwnd = _appWindow->getHandle();

   setForegroundWindow(hwnd);

   if (::IsIconic(hwnd))
      ::ShowWindowAsync(hwnd, Settings::appMaximized ? SW_MAXIMIZE : SW_SHOWNORMAL);

   IDE::onDebuggerStep(ns, source, info);
}

void WIN32IDE :: onDebuggerStop(bool broken)
{
   HWND hwnd = _appWindow->getHandle();

   setForegroundWindow(hwnd);

   if (::IsIconic(hwnd))
      ::ShowWindowAsync(hwnd, Settings::appMaximized ? SW_MAXIMIZE : SW_SHOWNORMAL);

   IDE::onDebuggerStop(broken);
}

void WIN32IDE :: onRClick(NMHDR* notification)
{
   if (_contextBrowser->checkHandle(notification->hwndFrom)) {
      DWORD dwpos = ::GetMessagePos();

      HTREEITEM item = ((Win32ContextBrowser*)_contextBrowser)->hitTest(LOWORD(dwpos), HIWORD(dwpos));
      if (item) {
         _contextBrowser->select(item);
      }
      ((Win32ContextBrowser*)_contextBrowser)->showContextMenu(_appWindow->getHandle(), LOWORD(dwpos), HIWORD(dwpos));
   }
}

void WIN32IDE :: onClientChanged(int code, NMHDR* notification)
{
   switch(code) {
      case TABCHANGED_NOTIFY:
         onTabChanged(notification->hwndFrom, -1);
         break;
      case VIEWCHANGED_NOTIFY:
         onCursorChange();
         onFrameChange();
         break;
   }
}

void WIN32IDE :: onContextMenu(ContextMenuNMHDR* notification)
{
   _mainFrame->showContextMenu(notification->x, notification->y);
}

void WIN32IDE :: onTabChanged(HWND wnd, int index)
{
   if (_mainFrame->checkHandle(wnd)) {
      _mainFrame->onTabChange(index);

      _windowList.add(_mainFrame->getDocumentPath(index));

      _mainFrame->setFocus();
      onFrameChange();
      onDocIncluded();
   }
   else if (_outputBar->checkHandle(wnd)) {
      _outputBar->onTabChange(index);

      onFrameChange();
   }
}

void WIN32IDE :: onActivate()
{
   if (_mainFrame)
      _mainFrame->setFocus();
}

WIN32IDE :: WIN32IDE(HINSTANCE instance, AppDebugController* debugController)
   : IDE(debugController), controls(NULL, _ELENA_::freeobj)
{
   this->instance = instance;

   // create main window & menu
   _appWindow = new MainWindow(instance, _T("IDE"), this);
   _appMenu = new Menu(instance, IDR_IDE_ACCELERATORS, ::GetMenu(_appWindow->getHandle()));
   contextMenu.create(8, contextMenuInfo);

   _appToolBar = new ToolBar(_appWindow, 16, AppToolBarButtonNumber, AppToolBarButtons);
   _statusBar = new StatusBar(_appWindow, 5, StatusBarWidths);
   _outputBar = createOutputBar();
   _contextBrowser = createContextBrowser();

   _mainFrame = new EditFrame(_appWindow, true, &contextMenu);
   TextView* textView = new TextView(_mainFrame, 5, 28, 400, 400);
   _mainFrame->populate(textView);
   textView->setReceptor(_appWindow);

   _mainFrame->show();
   _appToolBar->show();
   _statusBar->show();

   _appWindow->_getLayoutManager()->setAsTop(_appToolBar);
   _appWindow->_setStatusBar(_statusBar);
   _appWindow->_getLayoutManager()->setAsClient(_mainFrame);

   controls.add(_mainFrame);
   controls.add(textView);
   controls.add(_statusBar);
   controls.add(_appToolBar);
   controls.add(_appMenu);
   controls.add(_appWindow);

   // initialize recent files / projects / windows manager
   _recentFiles.assign(_appMenu);
   _recentProjects.assign(_appMenu);
   _windowList.assign(_appMenu);
}

// --- Win32AppDebugController ---

void Win32AppDebugController :: _notify(int code)
{
   NMHDR notification;

   notification.code = code;
   notification.hwndFrom = NULL;

   ::SendMessage(_receptor->getHandle(), WM_NOTIFY, 0, (LPARAM)&notification);
}

void Win32AppDebugController :: _notify(int code, const wchar_t* message, int param)
{
   MessageNMHDR notification;

   notification.nmhrd.code = code;
   notification.nmhrd.hwndFrom = NULL;
   notification.message = message;
   notification.param = param;

   ::SendMessage(_receptor->getHandle(), WM_NOTIFY, 0, (LPARAM)&notification);
}

void Win32AppDebugController :: _notify(int code, const wchar_t* message, const wchar_t* param)
{
   Message3NMHDR notification;

   notification.nmhrd.code = code;
   notification.nmhrd.hwndFrom = NULL;
   notification.message = message;
   notification.param = param;

   ::SendMessage(_receptor->getHandle(), WM_NOTIFY, 0, (LPARAM)&notification);
}

void Win32AppDebugController :: _notify(int code, const wchar_t* message, int param1, int param2)
{
   Message2NMHDR notification;

   notification.nmhrd.code = code;
   notification.nmhrd.hwndFrom = NULL;
   notification.message = message;
   notification.param1 = param1;
   notification.param2 = param2;

   ::SendMessage(_receptor->getHandle(), WM_NOTIFY, 0, (LPARAM)&notification);
}

void Win32AppDebugController :: _notify(int code, const wchar_t* ns, const wchar_t* source, HighlightInfo info)
{
   LineInfoNMHDR notification;

   notification.nmhrd.code = code;
   notification.nmhrd.hwndFrom = NULL;
   notification.ns = ns;
   notification.file = source;
   notification.position = info;

   ::SendMessage(_receptor->getHandle(), WM_NOTIFY, 0, (LPARAM)&notification);
}

size_t Win32AppDebugController :: findEntryPoint(const tchar_t* programPath)
{
   return _ELENA_::PEHelper::findEntryPoint(programPath);
}

void Win32AppDebugController :: onInitBreakpoint()
{
   if (_debugInfoPtr == 0) {
      // define if it is a vm client or stand-alone

      // !! hard-coded address; better propely to load part of NT_HEADER to correctly get bss address
      size_t rdata = _debugger.Context()->readDWord(0x4000D0);

      // load Executable image
      char signature[0x10];
      _debugger.Context()->readDump(0x400000 + rdata, signature, strlen(ELENACLIENT_SIGNITURE) + 1);

      if (strcmp(signature, ELENACLIENT_SIGNITURE) == 0) {
         if (_vmHook == 0) {
            _vmHook = _debugger.Context()->readDWord(0x400000 + rdata + _ELENA_::align(strlen(ELENACLIENT_SIGNITURE),4));

            // enable debug mode
            _debugger.Context()->writeDWord(_vmHook, -1);

            // continue debugging
            _events.setEvent(DEBUG_RESUME);
            return;
         }
         // load VM debug section address
         else _debugInfoPtr = _debugger.Context()->readDWord(_vmHook + 4);
      }
      else if (strncmp(signature, ELENA_SIGNITURE, strlen(ELENA_SIGNITURE)) == 0) {
         DebugReader reader(&_debugger, 0x400000, 0);

         _ELENA_::PEHelper::seekSection(reader, ".debug", _debugInfoPtr);
      }
      // !! notify if the executable is not supported
   }

   AppDebugController::onInitBreakpoint();

   _notify(IDE_DEBUGGER_HOOK);
}

