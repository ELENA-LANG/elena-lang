//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Implementation File
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <tchar.h>

#include "windows/winide.h"

#include "eng/messages.h"

#include "windows/wintreeview.h"
#include "windows/wintabbar.h"
#include "windows/wintoolbar.h"
#include "windows/winmenu.h"
#include "windows/wintextframe.h"

#include <windows/Resource.h>

using namespace elena_lang;

// --- Clipboard ---

Clipboard :: Clipboard(ControlBase* owner)
{
   this->_owner = owner;
}

bool Clipboard :: begin()
{
   return (::OpenClipboard(_owner->handle()) != 0);
}

void Clipboard :: clear()
{
   ::EmptyClipboard();
}

void Clipboard :: copy(HGLOBAL buffer)
{
   ::SetClipboardData(CF_UNICODETEXT, buffer);
}

HGLOBAL Clipboard :: get()
{
   return ::GetClipboardData(CF_UNICODETEXT);
}

void Clipboard :: end()
{
   ::CloseClipboard();
}

HGLOBAL Clipboard :: createBuffer(size_t size)
{
   return ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, (size + 1) * sizeof(wchar_t));
}

wchar_t* Clipboard :: allocateBuffer(HGLOBAL buffer)
{
   return (wchar_t*)::GlobalLock(buffer);
}

void Clipboard :: freeBuffer(HGLOBAL buffer)
{
   ::GlobalUnlock(buffer);
}

bool Clipboard :: copyToClipboard(DocumentView* docView)
{
   if (begin()) {
      clear();

      HGLOBAL buffer = createBuffer(docView->getSelectionLength());
      wchar_t* text = allocateBuffer(buffer);

      docView->copySelection(text);

      freeBuffer(buffer);
      copy(buffer);

      end();

      return true;
   }

   return false;
}

void Clipboard :: pasteFromClipboard(DocumentChangeStatus& status, DocumentView* docView)
{
   if (begin()) {
      HGLOBAL buffer = get();
      wchar_t* text = allocateBuffer(buffer);
      if (!emptystr(text)) {
         docView->insertLine(status, text, getlength(text));

         freeBuffer(buffer);
      }

      end();
   }
}

bool Clipboard :: isAvailable()
{
   return (::IsClipboardFormatAvailable(CF_UNICODETEXT) == TRUE);
}

// --- IDENotificationFormatter ---

void IDENotificationFormatter :: sendTextViewModelEvent(TextViewModelEvent* event, WindowApp* app)
{
   TextViewModelNMHDR nw = { };
   nw.docStatus = event->changeStatus;
   nw.status = event->status;

   app->notify(EVENT_TEXTVIEW_MODEL_CHANGED, (NMHDR*)&nw);
}

void IDENotificationFormatter :: sendTextFrameSelectionEvent(SelectionEvent* event, WindowApp* app)
{
   SelectionNMHDR nw = { };
   nw.index = event->Index();
   nw.status = event->status;

   app->notify(EVENT_TEXTFRAME_SELECTION_CHANGED, (NMHDR*)&nw);
}

void IDENotificationFormatter :: sendCompilationEndEvent(SelectionEvent* event, WindowApp* app)
{
   SelectionNMHDR nw = { };
   nw.index = event->Index();
   nw.status = event->status;

   app->notify(EVENT_COMPILATION_END, (NMHDR*)&nw);
}

void IDENotificationFormatter :: sendErrorListSelEvent(SelectionEvent* event, WindowApp* app)
{
   SelectionNMHDR nw = { };
   nw.index = event->Index();
   nw.status = event->status;

   app->notify(EVENT_ERRORLIST_SELECTION, (NMHDR*)&nw);
}

void IDENotificationFormatter :: sendProjectViewSelectionEvent(ParamSelectionEvent* event, WindowApp* app)
{
   ParamSelectionNMHDR nw = { };
   nw.param = event->Param();
   nw.status = event->status;

   app->notify(EVENT_PROJECTVIEW_SELECTION_CHANGED, (NMHDR*)&nw);
}

void IDENotificationFormatter :: sendStartUpEvent(StartUpEvent* event, WindowApp* app)
{
   ModelNMHDR nw = { };
   nw.status = event->status;

   app->notify(EVENT_STARTUP, (NMHDR*)&nw);
}


void IDENotificationFormatter :: sendLayoutEvent(LayoutEvent* event, WindowApp* app)
{
   ModelNMHDR nw = { };
   nw.status = event->status;

   app->notify(EVENT_LAYOUT, (NMHDR*)&nw);
}

void IDENotificationFormatter :: sendTextContextMenuEvent(ContextMenuEvent* event, WindowApp* app)
{
   ContextMenuNMHDR nw = {};

   nw.x = event->X();
   nw.y = event->X();
   nw.hasSelection = event->HasSelection();

   app->notify(EVENT_TEXT_CONTEXTMENU, (NMHDR*)&nw);
}

void IDENotificationFormatter :: sendBrowseContextMenuEvent(BrowseEvent* event, WindowApp* app)
{
   BrowseNMHDR nw = {};

   nw.item = event->Item();
   nw.param = event->Param();

   app->notify(EVENT_BROWSE_CONTEXT, (NMHDR*)&nw);
}

void IDENotificationFormatter :: sendMessage(EventBase* event, WindowApp* app)
{
   switch (event->eventId()) {      
      case EVENT_TEXTVIEW_MODEL_CHANGED:
         sendTextViewModelEvent(dynamic_cast<TextViewModelEvent*>(event), app);
         break;
      case EVENT_TEXTFRAME_SELECTION_CHANGED:
         sendTextFrameSelectionEvent(dynamic_cast<SelectionEvent*>(event), app);
         break;
      case EVENT_STARTUP:
         sendStartUpEvent(dynamic_cast<StartUpEvent*>(event), app);
         break;
      case EVENT_LAYOUT:
         sendLayoutEvent(dynamic_cast<LayoutEvent*>(event), app);
         break;
      case EVENT_PROJECTVIEW_SELECTION_CHANGED:
         sendProjectViewSelectionEvent(dynamic_cast<ParamSelectionEvent*>(event), app);
         break;
      case EVENT_COMPILATION_END:
         sendCompilationEndEvent(dynamic_cast<SelectionEvent*>(event), app);
         break;
      case EVENT_ERRORLIST_SELECTION:
         sendErrorListSelEvent(dynamic_cast<SelectionEvent*>(event), app);
         break;
      case EVENT_TEXT_CONTEXTMENU:
         sendTextContextMenuEvent(dynamic_cast<ContextMenuEvent*>(event), app);
         break;
      case EVENT_BROWSE_CONTEXT:
         sendBrowseContextMenuEvent(dynamic_cast<BrowseEvent*>(event), app);
         break;
      default:
         assert(false);
         break;
   }
}

// --- IDEWindow ---

IDEWindow :: IDEWindow(wstr_t title, IDEController* controller, IDEModel* model, HINSTANCE instance, ViewFactoryBase* viewFactory) :
   SDIWindow(title), 
   fileDialog(instance,
      this, FileDialog::SourceFilter, 
      OPEN_FILE_CAPTION, 
      *model->projectModel.paths.lastPath),
   projectDialog(instance,
      this, FileDialog::ProjectFilter,
      OPEN_PROJECT_CAPTION,
      *model->projectModel.paths.lastPath),
   messageDialog(this),
   projectSettingsDialog(instance, this, &model->projectModel),
   findDialog(instance, this, false, &model->findModel),
   replaceDialog(instance, this, true, &model->findModel),
   gotoDialog(instance, this),
   windowDialog(instance, this, &model->sourceViewModel),
   clipboard(this),
   _windowList(controller, model->viewModel()),
   _recentFileList(controller, model, IDM_FILE_FILES),
   _recentProjectList(controller, model, IDM_FILE_PROJECTS),
   aboutDialog(instance, this),
   editorSettingsDialog(instance, this, model->viewModel()),
   _docViewListener(nullptr)
{
   this->_viewFactory = viewFactory;

   this->_instance = instance;
   this->_controller = controller;
   this->_model = model;
}

void IDEWindow :: onActivate()
{
   auto center = _layoutManager.getCenter();
   if (center)
      center->setFocus();
}

void IDEWindow :: newFile()
{
   _controller->doNewFile(_model);
}

void IDEWindow :: newProject()
{
   _controller->doNewProject(fileDialog, messageDialog, projectSettingsDialog, _model);
}

void IDEWindow :: openFile()
{
   _controller->doOpenFile(fileDialog, _model);
   _recentFileList.reload();
   _recentProjectList.reload();
}

void IDEWindow :: saveFile()
{
   _controller->doSaveFile(fileDialog, _model, false, true);
}

void IDEWindow::saveAll()
{
   _controller->doSaveAll(fileDialog, projectDialog, _model);
}

void IDEWindow :: saveProject()
{
   _controller->doSaveProject(fileDialog, projectDialog, _model, true);
}

void IDEWindow :: closeFile()
{
   _controller->doCloseFile(fileDialog, messageDialog, _model);
}

void IDEWindow :: closeAll()
{
   _controller->doCloseAll(fileDialog, messageDialog, _model);
}

void IDEWindow :: closeAllButActive()
{
   _controller->doCloseAllButActive(fileDialog, messageDialog, _model);
}

void IDEWindow :: openProject()
{
   _controller->doOpenProject(projectDialog, messageDialog, _model);
   _recentProjectList.reload();
}

void IDEWindow :: closeProject()
{
   _controller->doCloseProject(projectDialog, messageDialog, _model);

   //_controller->onLayoutchange();
}

void IDEWindow :: exit()
{
   if(_controller->doExit(fileDialog, messageDialog, _model)) {
      SDIWindow::exit();
   }
}

void IDEWindow :: undo()
{
   _controller->sourceController.undo(_model->viewModel());
}

void IDEWindow :: redo()
{
   _controller->sourceController.redo(_model->viewModel());
}

bool IDEWindow :: copyToClipboard()
{
   return _controller->sourceController.copyToClipboard(_model->viewModel(), &clipboard);
}

void IDEWindow :: pasteFromClipboard()
{
   _controller->sourceController.pasteFromClipboard(_model->viewModel(), &clipboard);
}

void IDEWindow :: deleteText()
{
   _controller->sourceController.deleteText(_model->viewModel());
}

void IDEWindow :: commentText()
{
   wchar_t str[3] = _T("//");

   _controller->sourceController.insertBlockText(_model->viewModel(), str, 2);
}

void IDEWindow :: uncommentText()
{
   wchar_t str[3] = _T("//");

   _controller->sourceController.deleteBlockText(_model->viewModel(), str, 2);
}

void IDEWindow :: selectAll()
{
   _controller->sourceController.selectAll(_model->viewModel());
}

void IDEWindow :: trim()
{
   _controller->sourceController.trim(_model->viewModel());
}

void IDEWindow :: eraseLine()
{
   _controller->sourceController.eraseLine(_model->viewModel());
}

void IDEWindow :: duplicateLine()
{
   _controller->sourceController.duplicateLine(_model->viewModel());
}

void IDEWindow :: lowerCase()
{
   _controller->sourceController.lowerCase(_model->viewModel());
}

void IDEWindow :: upperCase()
{
   _controller->sourceController.upperCase(_model->viewModel());
}

void IDEWindow :: search()
{
   if (!_controller->doSearch(findDialog, _model)) {
      messageDialog.info(NOT_FOUND_TEXT);
   }
}

void IDEWindow :: searchNext()
{
   if (!_controller->doSearchNext(_model)) {
      messageDialog.info(NOT_FOUND_TEXT);
   }
}

void IDEWindow :: goToLine()
{
   _controller->doGoToLine(gotoDialog, _model);
}

void IDEWindow :: replace()
{
   if (!_controller->doReplace(replaceDialog, messageDialog, _model)) {
      messageDialog.info(NOT_FOUND_TEXT);
   }
}

void IDEWindow :: includeFile()
{
   _controller->doInclude(_model);
}

void IDEWindow :: toggleProjectView(bool open)
{
   GUIControlBase* projectView = _children[_model->ideScheme.projectView];

   if (open) {
      projectView->show();
   }
   else projectView->hide();
}

void IDEWindow :: openResultTab(int controlIndex)
{
   TabBar* resultBar = (TabBar*)_children[_model->ideScheme.resultControl];

   if(!resultBar->selectTabChild((ControlBase*)_children[controlIndex])) {
      resultBar->addTabChild(_model->ideScheme.captions.get(controlIndex), (ControlBase*)_children[controlIndex]);
      resultBar->selectTabChild((ControlBase*)_children[controlIndex]);
   }

   if (!resultBar->visible()) {
      resultBar->show();

      onLayoutChange();
   }
}

void IDEWindow :: closeResultTab(int controlIndex)
{
   _children[controlIndex]->hide();

   TabBar* resultBar = (TabBar*)_children[_model->ideScheme.resultControl];

   resultBar->removeTabChild((ControlBase*)_children[controlIndex]);

   if (resultBar->empty()) {
      resultBar->hide();

      onLayoutChange();
   }
   else resultBar->selectTab(0);
}

void IDEWindow :: toggleWindow(int child_id)
{
   if (child_id == _model->ideScheme.projectView) {
      if (!_children[_model->ideScheme.projectView]->visible()) {
         openProject();
      }
      else closeProject();
   }
}

bool IDEWindow :: toggleTabBarWindow(int child_id)
{
   if (!_children[child_id]->visible()) {
      openResultTab(child_id);

      return true;
   }

   closeResultTab(child_id);
   return false;
}

void IDEWindow :: setChildFocus(int controlIndex)
{
   _children[controlIndex]->setFocus();
}

void IDEWindow :: onComilationStart()
{
   updateCompileMenu(false, false);

   openResultTab(_model->ideScheme.compilerOutputControl);

   ((ControlBase*)_children[_model->ideScheme.compilerOutputControl])->clearValue();
}

void IDEWindow :: onCompilationEnd(int exitCode)
{
   wchar_t* output = ((ControlBase*)_children[_model->ideScheme.compilerOutputControl])->getValue();
   ControlBase* messageLog = (ControlBase*)_children[_model->ideScheme.errorListControl];

   _controller->onCompilationCompletion(_model, exitCode, output, dynamic_cast<ErrorLogBase*>(messageLog));

   freestr(output);
}

void IDEWindow :: onErrorHighlight(int index)
{
   ErrorLogBase* resultBar = dynamic_cast<ErrorLogBase*>(_children[_model->ideScheme.errorListControl]);

   auto messageInfo = resultBar->getMessage(index);
   if (!messageInfo.path.empty())
      _controller->highlightError(_model, messageInfo.row, messageInfo.column, messageInfo.path);
}

void IDEWindow :: onDebugWatch()
{
   openResultTab(_model->ideScheme.debugWatch);

   ContextBrowserBase* contextBrowser = dynamic_cast<ContextBrowserBase*>(_children[_model->ideScheme.debugWatch]);

   _controller->refreshDebugContext(contextBrowser, _model);

   contextBrowser->expandRootNode();
}

void IDEWindow :: onDebugWatchBrowse(BrowseNMHDR* rec)
{
   if (rec->param) {
      ContextBrowserBase* contextBrowser = dynamic_cast<ContextBrowserBase*>(_children[_model->ideScheme.debugWatch]);
      _controller->refreshDebugContext(contextBrowser, _model, rec->item, rec->param);
   }
}

void IDEWindow :: enableMenuItemById(int id, bool doEnable, bool toolBarItemAvailable)
{
   MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);

   menu->enableMenuItemById(id, doEnable);
   if (toolBarItemAvailable) {
      ToolBar* toolBar = dynamic_cast<ToolBar*>(_children[_model->ideScheme.toolBarControl]);

      toolBar->enableItemById(id, doEnable);
   }
}

void IDEWindow :: updateCompileMenu(bool compileEnable, bool debugEnable)
{
   enableMenuItemById(IDM_PROJECT_COMPILE, compileEnable, false);
   enableMenuItemById(IDM_PROJECT_OPTION, compileEnable, false);

   enableMenuItemById(IDM_DEBUG_RUN, debugEnable, true);
   enableMenuItemById(IDM_DEBUG_STEPINTO, debugEnable, true);
   enableMenuItemById(IDM_DEBUG_STEPOVER, debugEnable, true);
   enableMenuItemById(IDM_DEBUG_RUNTO, debugEnable, false);
   enableMenuItemById(IDM_DEBUG_STOP, debugEnable, true);
}

void IDEWindow :: onProjectRefresh(bool empty)
{
   updateCompileMenu(!empty, !empty);

   enableMenuItemById(IDM_PROJECT_CLOSE, !empty, true);
   enableMenuItemById(IDM_FILE_SAVEPROJECT, !empty, false);
}

void IDEWindow :: onProjectChange(bool empty)
{
   TreeView* projectTree = dynamic_cast<TreeView*>(_children[_model->ideScheme.projectView]);

   projectTree->clear(nullptr);

   if (!empty) {
      TreeViewItem root = projectTree->insertTo(nullptr, *_model->projectModel.name, -1, true);

      PathString diskCriteria(":\\");

      int srcIndex = 0;
      wchar_t buffer[IDENTIFIER_LEN + 1];
      for (auto it = _model->projectModel.sources.start(); !it.eof(); ++it) {
         path_t src = *it;
         size_t src_len = src.length_pos();

         // remove the disk name
         size_t index = src.findStr(*diskCriteria);
         if (index != NOTFOUND_POS) {
            src = src + index + 2;
         }

         TreeViewItem parent = root;
         size_t start = 0;
         while (start < src_len) {
            size_t end = src.findSub(start, PATH_SEPARATOR, src_len);
            PathString name;
            name.append(src + start, end - start);

            TreeViewItem current = projectTree->getChild(parent);
            while (current != nullptr) {
               size_t len = projectTree->readCaption(current, buffer, IDENTIFIER_LEN);
               if (!(*name).compareSub(buffer, 0, len)) {
                  current = projectTree->getNext(current);
               }
               else break;
            }

            if (current == nullptr) {
               current = projectTree->insertTo(parent, *name, end == src_len ? srcIndex : NOTFOUND_POS, end != src_len ? true : false);
            }
            parent = current;

            start = end + 1;
         }

         srcIndex++;
      }

      projectTree->expand(root);
   }

   toggleProjectView(!empty);

   onProjectRefresh(empty);
}

bool IDEWindow :: onCommand(int command)
{
   switch (command) {
      case IDM_FILE_NEW:
         newFile();
         break;
      case IDM_PROJECT_NEW:
         newProject();
         break;
      case IDM_FILE_OPEN:
         openFile();
         break;
      case IDM_PROJECT_OPEN:
         openProject();
         break;
      case IDM_FILE_SAVE:
         saveFile();
         break;
      case IDM_FILE_SAVEALL:
         saveAll();
         break;
      case IDM_FILE_SAVEPROJECT:
         saveProject();
         break;
      case IDM_FILE_CLOSE:
         closeFile();
         break;
      case IDM_FILE_CLOSEALL:
         closeAll();
         break;
      case IDM_FILE_CLOSEALLBUT:
         closeAllButActive();
         break;
      case IDM_PROJECT_CLOSE:
         closeProject();
         break;
      case IDM_FILE_EXIT:
         exit();
         break;
      case IDM_EDIT_UNDO:
         undo();
         break;
      case IDM_EDIT_REDO:
         redo();
         break;
      case IDM_EDIT_CUT:
         if (copyToClipboard())
            deleteText();
         break;
      case IDM_EDIT_COPY:
         copyToClipboard();
         break;
      case IDM_EDIT_PASTE:
         pasteFromClipboard();
         break;
      case IDM_EDIT_DELETE:
         deleteText();
         break;
      case IDM_EDIT_COMMENT:
         commentText();
         break;
      case IDM_EDIT_UNCOMMENT:
         uncommentText();
         break;
      case IDM_EDIT_SELECTALL:
         selectAll();
         break;
      case IDM_EDIT_TRIM:
         trim();
         break;
      case IDM_EDIT_ERASELINE:
         eraseLine();
         break;
      case IDM_EDIT_DUPLICATE:
         duplicateLine();
         break;
      case IDM_EDIT_INDENT:
         _controller->doIndent(_model);
         break;
      case IDM_EDIT_OUTDENT:
         _controller->doOutdent(_model);
         break;
      case IDM_EDIT_LOWERCASE:
         lowerCase();
         break;
      case IDM_EDIT_UPPERCASE:
         upperCase();
         break;
      case IDM_SEARCH_FIND:
         search();
         break;
      case IDM_SEARCH_FINDNEXT:
         searchNext();
         break;
      case IDM_SEARCH_GOTOLINE:
         goToLine();
         break;
      case IDM_SEARCH_REPLACE:
         replace();
         break;
      case IDM_PROJECT_COMPILE:
         _controller->doCompileProject(fileDialog, projectDialog, _model);
         break;
      case IDM_PROJECT_INCLUDE:
         includeFile();
         break;
      case IDM_PROJECT_OPTION:
         _controller->doChangeProject(projectSettingsDialog, _model);
         break;
      case IDM_DEBUG_RUN:
         _controller->doDebugAction(_model, DebugAction::Run);
         break;
      case IDM_DEBUG_STEPOVER:
         _controller->doDebugAction(_model, DebugAction::StepOver);
         break;
      case IDM_DEBUG_STEPINTO:
         _controller->doDebugAction(_model, DebugAction::StepInto);
         break;
      case IDM_DEBUG_RUNTO:
         _controller->doDebugAction(_model, DebugAction::RunTo);
         break;
      case IDM_DEBUG_STOP:
         _controller->doDebugStop(_model);
         break;
      case IDM_DEBUG_BREAKPOINT:
         _controller->toggleBreakpoint(_model, -1);
         break;
      case IDM_WINDOW_NEXT:
         _controller->doSelectNextWindow(_model);
         break;
      case IDM_WINDOW_PREVIOUS:
         _controller->doSelectPrevWindow(_model);
         break;
      case IDM_VIEW_WATCH:
         toggleTabBarWindow(_model->ideScheme.debugWatch);
         break;
      case IDM_VIEW_PROJECTVIEW:
         toggleWindow(_model->ideScheme.projectView);
         break;
      case IDM_VIEW_OUTPUT:
         toggleTabBarWindow(_model->ideScheme.compilerOutputControl);
         break;
      case IDM_VIEW_VMCONSOLE:
         if (toggleTabBarWindow(_model->ideScheme.vmConsoleControl)) {
            _controller->doStartVMConsole(_model);
         }
         else _controller->doStopVMConsole();
         break;
      case IDM_VIEW_MESSAGES:
         toggleTabBarWindow(_model->ideScheme.errorListControl);
         break;
      case IDM_HELP_API:
         openHelp();
         break;
      case IDM_HELP_ABOUT:
         showAbout();
         break;
      case IDM_DEBUG_INSPECT:
         refreshDebugNode();
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
         _windowList.select(command - IDM_WINDOW_FIRST + 1);
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
      {
         PathString path(_recentFileList.getPath(command - IDM_FILE_FILES));
         _controller->doOpenFile(_model, *path);
         _recentFileList.reload();
         break;
      }
      case IDM_FILE_PROJECTS_1:
      case IDM_FILE_PROJECTS_2:
      case IDM_FILE_PROJECTS_3:
      case IDM_FILE_PROJECTS_4:
      case IDM_FILE_PROJECTS_5:
      case IDM_FILE_PROJECTS_6:
      case IDM_FILE_PROJECTS_7:
      case IDM_FILE_PROJECTS_8:
      case IDM_FILE_PROJECTS_9:
      {
         PathString path(_recentProjectList.getPath(command - IDM_FILE_PROJECTS));
         _controller->doOpenProject(projectDialog, messageDialog, _model, *path);
         _recentProjectList.reload();
         break;
      }
      case IDM_FILE_FILES_CLEAR:
         _model->projectModel.lastOpenFiles.clear();
         _recentFileList.reload();
         break;
      case IDM_FILE_PROJECTS_CLEAR:
         _model->projectModel.lastOpenProjects.clear();
         _recentProjectList.reload();
         break;
      case IDM_EDITOR_OPTIONS:
         _controller->doConfigureEditorSettings(editorSettingsDialog, _model);
         break;
      case IDM_WINDOW_WINDOWS:
         _controller->doSelectWindow(fileDialog, messageDialog, windowDialog, _model);
         break;
      default:
         return false;
   }

   return true;
}

void IDEWindow :: refreshDebugNode()
{
   dynamic_cast<ContextBrowserBase*>(_children[_model->ideScheme.debugWatch])->refreshCurrentNode();
}

void IDEWindow :: onTabSelChanged(HWND wnd)
{
   for (size_t i = 0; i < _childCounter; i++) {
      if (_children[i]->checkHandle(wnd)) {
         ((ControlBase*)_children[i])->onSelChanged();
         break;
      }
   }
}

void IDEWindow :: onTreeSelChanged(HWND wnd)
{
   for (size_t i = 0; i < _childCounter; i++) {
      if (_children[i]->checkHandle(wnd)) {
         ((ControlBase*)_children[i])->onSelChanged();
         break;
      }
   }
}

void IDEWindow :: onTreeItemExpanded(NMTREEVIEWW* rec)
{
   for (size_t i = 0; i < _childCounter; i++) {
      if (_children[i]->checkHandle(rec->hdr.hwndFrom)) {
         ((TreeView*)_children[i])->onItemExpand(rec->itemNew.hItem);
         break;
      }
   }
}

void IDEWindow :: onDoubleClick(NMHDR* hdr)
{
   for (size_t i = 0; i < _childCounter; i++) {
      if (_children[i]->checkHandle(hdr->hwndFrom)) {
         ((ControlBase*)_children[i])->onDoubleClick(hdr);
         break;
      }
   }
}

void IDEWindow :: onDebugWatchRClick(size_t controlIndex)
{
   DWORD dwpos = ::GetMessagePos();
   Point p(LOWORD(dwpos), HIWORD(dwpos));

   TreeView* treeView = ((TreeView*)_children[controlIndex]);

   HTREEITEM item = treeView->hitTest(p.x, p.x);
   if (item) {
      treeView->select(item);
   }

   ContextMenu* menu = static_cast<ContextMenu*>(_children[_model->ideScheme.debugContextMenu]);

   menu->show(_handle, p);
}

void IDEWindow :: onRClick(NMHDR* hdr)
{
   for (size_t i = 0; i < _childCounter; i++) {
      if (_children[i]->checkHandle(hdr->hwndFrom)) {
         if (i == _model->ideScheme.debugWatch) {
            onDebugWatchRClick(i);
         }
         break;
      }
   }
}

void IDEWindow :: onContextMenu(ContextMenuNMHDR* rec)
{
   Point p(rec->x, rec->y);

   ContextMenu* menu = static_cast<ContextMenu*>(_children[_model->ideScheme.editorContextMenu]);

   menu->enableMenuItemById(IDM_EDIT_CUT, rec->hasSelection);
   menu->enableMenuItemById(IDM_EDIT_COPY, rec->hasSelection);
   menu->enableMenuItemById(IDM_EDIT_PASTE, Clipboard::isAvailable());

   menu->show(_handle, p);
}

void IDEWindow :: onDebugResult(int code)
{
   switch (code) {
      case DEBUGGER_STOPPED:
      {
         _controller->onDebuggerStop(_model);

         MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);
         menu->enableMenuItemById(IDM_DEBUG_STOP, true);
         break;
      }
      default:
         break;
   }
}

void IDEWindow :: onChildRefresh(int controlId)
{
   _children[controlId]->refresh();
}

void IDEWindow :: onStatusBarChange()
{
   _children[_model->ideScheme.statusBar]->refresh();
}

void IDEWindow :: onIDEStatusChange(ModelNMHDR* rec)
{
   if (test(rec->status, STATUS_DOC_READY)) {
      _model->status = IDEStatus::Ready;

      onStatusBarChange();
   }
   else if (test(rec->status, STATUS_DEBUGGER_STOPPED)) {
      _model->status = IDEStatus::Stopped;

      onStatusBarChange();
      updateCompileMenu(false, true);
   }   
   else if (test(rec->status, STATUS_DEBUGGER_RUNNING)) {
      _model->status = IDEStatus::Running;

      onStatusBarChange();
      updateCompileMenu(false, false);
   }
   else if (test(rec->status, STATUS_DEBUGGER_FINISHED)) {
      _model->status = IDEStatus::Stopped;

      onStatusBarChange();
      updateCompileMenu(true, true);
   }
   else if (test(rec->status, STATUS_STATUS_CHANGED)) {
      onStatusBarChange();
   }

   if (test(rec->status, STATUS_PROJECT_CHANGED)) {
      onProjectChange(_model->projectModel.empty);
   }
   else if (test(rec->status, STATUS_PROJECT_REFRESH)) {
      onProjectRefresh(_model->projectModel.empty);
   }

   if (test(rec->status, STATUS_FRAME_VISIBILITY_CHANGED)) {
      if (_model->sourceViewModel.isAssigned()) {
         _children[_model->ideScheme.textFrameId]->show();
         _children[_model->ideScheme.textFrameId]->setFocus();
      }
      else _children[_model->ideScheme.textFrameId]->hide();
   }

   if (test(rec->status, STATUS_COMPILING)) {
      onComilationStart();
   }

   if (test(rec->status, STATUS_LAYOUT_CHANGED)) {
      onLayoutChange();
   }

   if (test(rec->status, STATUS_WITHERRORS)) {
      openResultTab(_model->ideScheme.errorListControl);
   }

   if (test(rec->status, STATUS_DEBUGGER_NOSOURCE)) {
      onDebuggerSourceNotFound();
   }
   else if (test(rec->status, STATUS_DEBUGGER_STEP)) {
      MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);
      menu->enableMenuItemById(IDM_DEBUG_STOP, true);

      onDebugWatch();
   }
}

void IDEWindow :: onTextModelChange(TextViewModelNMHDR* rec)
{
   onDocumentUpdate(rec->docStatus);
   onIDEStatusChange(rec);
}

void IDEWindow :: onTextFrameSel(SelectionNMHDR* rec)
{
   _controller->onDocSelection(_model, rec->index);
}

void IDEWindow :: onProjectViewSel(ParamSelectionNMHDR* rec)
{
   _controller->doOpenProjectSourceByIndex(_model, (int)rec->param);

   _children[_model->ideScheme.textFrameId]->setFocus();
}

void IDEWindow :: onLayoutChange()
{
   bool empty = _model->sourceViewModel.getDocumentCount() == 0;

   MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);
   menu->checkMenuItemById(IDM_VIEW_OUTPUT, _children[_model->ideScheme.compilerOutputControl]->visible());
   menu->checkMenuItemById(IDM_VIEW_PROJECTVIEW, _children[_model->ideScheme.projectView]->visible());
   menu->checkMenuItemById(IDM_VIEW_MESSAGES, _children[_model->ideScheme.errorListControl]->visible());
   menu->checkMenuItemById(IDM_VIEW_WATCH, _children[_model->ideScheme.debugWatch]->visible());
   menu->checkMenuItemById(IDM_VIEW_VMCONSOLE, _children[_model->ideScheme.vmConsoleControl]->visible());

   enableMenuItemById(IDM_FILE_SAVE, !empty, true);
   enableMenuItemById(IDM_FILE_SAVEALL, !empty, true);
   enableMenuItemById(IDM_FILE_SAVEAS, !empty, false);
   enableMenuItemById(IDM_FILE_CLOSE, !empty, true);
   enableMenuItemById(IDM_FILE_CLOSEALL, !empty, false);

   if (empty) {
      enableMenuItemById(IDM_EDIT_UNDO, false, true);
      enableMenuItemById(IDM_EDIT_REDO, false, true);

      enableMenuItemById(IDM_EDIT_CUT, false, true);
      enableMenuItemById(IDM_EDIT_COPY, false, true);
      enableMenuItemById(IDM_EDIT_PASTE, false, true);

      enableMenuItemById(IDM_PROJECT_INCLUDE, false, false);
   }
   else {
      enableMenuItemById(IDM_EDIT_PASTE, true, true);

      enableMenuItemById(IDM_PROJECT_INCLUDE, true, false);
   }

   enableMenuItemById(IDM_EDIT_DELETE, !empty, false);
   enableMenuItemById(IDM_EDIT_COMMENT, !empty, false);
   enableMenuItemById(IDM_EDIT_UNCOMMENT, !empty, false);

   enableMenuItemById(IDM_EDIT_DUPLICATE, !empty, false);
   enableMenuItemById(IDM_EDIT_SELECTALL, !empty, false);
   enableMenuItemById(IDM_EDIT_ERASELINE, !empty, false);
   enableMenuItemById(IDM_EDIT_INDENT, !empty, false);
   enableMenuItemById(IDM_EDIT_OUTDENT, !empty, false);
   enableMenuItemById(IDM_EDIT_TRIM, !empty, false);
   enableMenuItemById(IDM_EDIT_UPPERCASE, !empty, false);
   enableMenuItemById(IDM_EDIT_LOWERCASE, !empty, false);

   enableMenuItemById(IDM_SEARCH_FIND, !empty, false);
   enableMenuItemById(IDM_SEARCH_FINDNEXT, !empty, false);
   enableMenuItemById(IDM_SEARCH_REPLACE, !empty, false);
   enableMenuItemById(IDM_SEARCH_GOTOLINE, !empty, false);

   enableMenuItemById(IDM_WINDOW_WINDOWS, !empty, false);
   enableMenuItemById(IDM_WINDOW_NEXT, !empty, false);
   enableMenuItemById(IDM_WINDOW_PREVIOUS, !empty, false);

   enableMenuItemById(IDM_DEBUG_CLEARBREAKPOINT, !empty, false);
   enableMenuItemById(IDM_DEBUG_BREAKPOINT, !empty, false);

   onResize();

   if (!empty)
      _children[_model->ideScheme.textFrameId]->refresh();
}

void IDEWindow :: onStartup(ModelNMHDR* rec)
{
   _controller->init(_model, rec->status);

   _recentFileList.assignList(&_model->projectModel.lastOpenFiles);
   _recentProjectList.assignList(&_model->projectModel.lastOpenProjects);

   onIDEStatusChange(rec);
}

void IDEWindow :: onNotify(NMHDR* hdr)
{
   switch (hdr->code) {
      case EVENT_TEXTVIEW_MODEL_CHANGED:
         onTextModelChange((TextViewModelNMHDR*)hdr);
         break;
      case EVENT_TEXTFRAME_SELECTION_CHANGED:
         onTextFrameSel((SelectionNMHDR*)hdr);
         break;
      case EVENT_PROJECTVIEW_SELECTION_CHANGED:
         onProjectViewSel((ParamSelectionNMHDR*)hdr);
         break;
      case EVENT_COMPILATION_END:
         onCompilationEnd(((SelectionNMHDR*)hdr)->index);
         break;
      case EVENT_ERRORLIST_SELECTION:
         onErrorHighlight(((SelectionNMHDR*)hdr)->index);
         break;
      case EVENT_STARTUP:
         onStartup((ModelNMHDR*)hdr);
         break;
      case EVENT_LAYOUT:
         onLayoutChange();
         break;
      case EVENT_TEXT_CONTEXTMENU:
         onContextMenu((ContextMenuNMHDR*)hdr);
         break;
      case NOTIFY_DEBUG_CONTEXT_EXPANDED:
         onDebugWatchBrowse((BrowseNMHDR*)hdr);
         break;
      case TCN_SELCHANGE:
         onTabSelChanged(hdr->hwndFrom);
         break;
      case NM_DBLCLK:
         onDoubleClick(hdr);
         break;
      case TVN_SELCHANGED:
         onTreeSelChanged(hdr->hwndFrom);
         break;
      case TVN_ITEMEXPANDING:
         onTreeItemExpanded((NMTREEVIEW*)hdr);
         break;
      case NM_RCLICK:
         onRClick(hdr);
         break;
      case TTN_GETDISPINFO:
         if (isTabToolTip(hdr->hwndFrom)) {
            onTabTip((NMTTDISPINFO*)hdr);
         }
         else onToolTip((NMTTDISPINFO*)hdr);
         break;
      default:
         break;
   }
}

void IDEWindow :: onDebuggerStart()
{
   ContextBrowserBase* contextBrowser = dynamic_cast<ContextBrowserBase*>(_children[_model->ideScheme.debugWatch]);

   contextBrowser->clearRootNode();
}

void IDEWindow :: onDebuggerHook()
{
   _controller->onDebuggerHook(_model);
}

void IDEWindow :: onDebuggerSourceNotFound()
{
   MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);
   menu->enableMenuItemById(IDM_DEBUG_STOP, true);

   _controller->onDebuggerNoSource(messageDialog, _model);
}

bool IDEWindow :: onClose()
{
   if (!_controller->onClose(fileDialog, messageDialog, _model))
      return false;

   return WindowBase::onClose();
}

void IDEWindow :: openHelp()
{
   PathString apiPath(_model->projectModel.paths.appPath);
   apiPath.combine(_T("..\\doc\\api\\index.html"));

   ShellExecute(NULL, _T("open"), *apiPath, nullptr, nullptr, SW_SHOW);
}

void IDEWindow :: showAbout()
{
   aboutDialog.show();
}

void IDEWindow :: onDocumentUpdate(DocumentChangeStatus& changeStatus)
{
   auto docInfo = _model->sourceViewModel.DocView();

   if (changeStatus.modifiedChanged) {
      ((TextViewFrame*)_children[_model->ideScheme.textFrameId])->onDocumentModeChanged(-1, docInfo->isModified());
   }

   if (changeStatus.selelectionChanged) {
      MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);

      bool isSelected = docInfo ? docInfo->hasSelection() : false;

      menu->enableMenuItemById(IDM_EDIT_COPY, isSelected);
      menu->enableMenuItemById(IDM_EDIT_CUT, isSelected);
      menu->enableMenuItemById(IDM_EDIT_COMMENT, isSelected);
      menu->enableMenuItemById(IDM_EDIT_UNCOMMENT, isSelected);
      menu->enableMenuItemById(IDM_EDIT_DELETE, isSelected);
   }
   if (changeStatus.textChanged) {
      MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);

      menu->enableMenuItemById(IDM_EDIT_UNDO, docInfo ? docInfo->canUndo() : false);
      menu->enableMenuItemById(IDM_EDIT_REDO, docInfo ? docInfo->canRedo() : false);
   }

   if (docInfo) {
      for (auto it = _docViewListener.start(); !it.eof(); ++it) {
         (*it)->onDocumentUpdate(changeStatus);
      }
   }
}

bool IDEWindow :: isTabToolTip(HWND handle)
{
   if (!_tabTTHandle)
      _tabTTHandle = ((ControlBase*)_children[_model->ideScheme.textFrameId])->getToolTipHandle();

   return _tabTTHandle == handle;
}

void IDEWindow :: onTabTip(NMTTDISPINFOW* tip)
{
   const wchar_t* path = _model->sourceViewModel.getDocumentPath((int)tip->hdr.idFrom + 1);

   tip->lpszText = (LPWSTR)path;
}

void IDEWindow :: onToolTip(NMTTDISPINFOW* toolTip)
{
   switch (toolTip->hdr.idFrom) {
      case IDM_FILE_NEW:
         toolTip->lpszText = (LPWSTR)HINT_NEW_FILE;
         break;
      case IDM_FILE_OPEN:
         toolTip->lpszText = (LPWSTR)HINT_OPEN_FILE;
         break;
      case IDM_FILE_SAVE:
         toolTip->lpszText = (LPWSTR)HINT_SAVE_FILE;
         break;
      case IDM_FILE_SAVEALL:
         toolTip->lpszText = (LPWSTR)HINT_SAVEALL;
         break;
      case IDM_FILE_CLOSE:
         toolTip->lpszText = (LPWSTR)HINT_CLOSE_FILE;
         break;
      case IDM_PROJECT_CLOSE:
         toolTip->lpszText = (LPWSTR)HINT_CLOSE_PROJECT;
         break;
      case IDM_EDIT_CUT:
         toolTip->lpszText = (LPWSTR)HINT_CUT;
         break;
      case IDM_EDIT_COPY:
         toolTip->lpszText = (LPWSTR)HINT_COPY;
         break;
      case IDM_EDIT_PASTE:
         toolTip->lpszText = (LPWSTR)HINT_PASTE;
         break;
      case IDM_EDIT_UNDO:
         toolTip->lpszText = (LPWSTR)HINT_UNDO;
         break;
      case IDM_EDIT_REDO:
         toolTip->lpszText = (LPWSTR)HINT_REDO;
         break;
      case IDM_DEBUG_RUN:
         toolTip->lpszText = (LPWSTR)HINT_RUN;
         break;
      case IDM_DEBUG_STOP:
         toolTip->lpszText = (LPWSTR)HINT_STOP;
         break;
      case IDM_DEBUG_STEPINTO:
         toolTip->lpszText = (LPWSTR)HINT_STEPINTO;
         break;
      case IDM_DEBUG_STEPOVER:
         toolTip->lpszText = (LPWSTR)HINT_STEPOVER;
         break;
      case IDM_DEBUG_GOTOSOURCE:
         toolTip->lpszText = (LPWSTR)HINT_GOTOSOURCE;
         break;
      default:
         break;
   }
}

void IDEWindow :: populate(size_t counter, GUIControlBase** children)
{
   SDIWindow::populate(counter, children);

   MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);

   _windowList.assign(menu);
   _recentFileList.assign(menu);
   _recentProjectList.assign(menu);

   _docViewListener.add(dynamic_cast<DocumentNotifier*>(_children[_model->ideScheme.textControlId]));
   _docViewListener.add(dynamic_cast<DocumentNotifier*>(_children[_model->ideScheme.statusBar]));
}

void IDEWindow :: onColorSchemeChange()
{
   _viewFactory->reloadStyles(_model->viewModel());
}
