//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Implementation File
//                                             (C)2021-2025, by Aleksey Rakov
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

bool Clipboard :: copyToClipboard(DocumentView* docView, bool selectionMode)
{
   if (begin()) {
      clear();

      HGLOBAL buffer = nullptr;
      if (selectionMode) {
         buffer = createBuffer(docView->getSelectionLength());
         wchar_t* text = allocateBuffer(buffer);

         docView->copySelection(text);
      }
      else {
         buffer = createBuffer(docView->getCurrentLineLength() + 2);
         wchar_t* text = allocateBuffer(buffer);

         docView->copyCurrentLine(text);
      }

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

void IDENotificationFormatter :: sendCompilationEndEvent(CompletionEvent* event, WindowApp* app)
{
   CompletionNMHDR nw = { };
   nw.exitCode = event->ExitCode();
   nw.postponedAction = event->PostpinedAction();
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
   nw.y = event->Y();
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

void IDENotificationFormatter :: sendSimpleEvent(SimpleEvent* event, WindowApp* app)
{
   NMHDR nw = {};

   app->notify(EVENT_TEXT_MARGINLICK, (NMHDR*)&nw);
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
         sendCompilationEndEvent(dynamic_cast<CompletionEvent*>(event), app);
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
      case EVENT_TEXT_MARGINLICK:
         sendSimpleEvent(dynamic_cast<SimpleEvent*>(event), app);
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
   fontSettingsDialog(instance, this),
   ideSettingsDialog(instance, this, model),
   debuggerSettingsDialog(instance, this, &model->projectModel),
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
   closeProject();

   _controller->doNewProject(_model);

   projectSettingsDialog.showModal();
}

void IDEWindow :: openFile()
{
   List<path_t, freepath> files(nullptr);
   if (fileDialog.openFiles(files) && _controller->doOpenFile(_model, files)) {
      _recentFileList.reload();
      _recentProjectList.reload();
   }
}

void IDEWindow :: saveFile(int index)
{
   if (_controller->ifFileUnnamed(_model, index)) {
      PathString path;
      if (!fileDialog.saveFile(_T("l"), path))
         return;

      _controller->doSaveFile(_model, index, true, *path);
   }
   else _controller->doSaveFile(_model, index, true);
}

void IDEWindow :: saveFileAs(int index)
{
   PathString path;
   if (!fileDialog.saveFile(_T("l"), path))
      return;

   _controller->doSaveFile(_model, index, true, *path);
}

void IDEWindow :: saveAll()
{
   for (int index = 1; index <= (int)_model->sourceViewModel.getDocumentCount(); index++) {
      if (_controller->ifFileNotSaved(_model, index) || _controller->ifFileUnnamed(_model, index))
         saveFile(index);
   }

   saveProject(false);
}

void IDEWindow :: saveProject(bool saveAsMode)
{
   if (_controller->ifProjectUnnamed(_model) || saveAsMode) {
      PathString path;
      if (!projectDialog.saveFile(_T("prj"), path))
         return;

      _controller->doSaveProject(_model, *path);
   }
   else if (_controller->ifProjectNotSaved(_model)) {
      _controller->doSaveProject(_model);
   }
}

bool IDEWindow :: saveBeforeClose(int index)
{
   if (_controller->ifFileUnnamed(_model, index)) {
      PathString path;
      if (!fileDialog.saveFile(_T("l"), path)) {
         auto result = messageDialog.question(QUESTION_CLOSE_UNSAVED);

         if (result != MessageDialogBase::Answer::Yes)
            return false;
      }
      else _controller->doSaveFile(_model, index, true, *path);
   }
   else if (_controller->ifFileNotSaved(_model, index)) {
      path_t path = _model->sourceViewModel.getDocumentPath(index);

      auto result = messageDialog.question(
         QUESTION_SAVE_FILECHANGES, path);

      if (result == MessageDialogBase::Answer::Cancel) {
         return false;
      }
      else if (result == MessageDialogBase::Answer::Yes) {
         PathString path;
         if (fileDialog.saveFile(_T("l"), path)) {
            _controller->doSaveFile(_model, index, true, *path);
         }
         else return false;
      }
   }

   return true;
}

void IDEWindow :: closeFile(int index)
{
   if (saveBeforeClose(index))
      _controller->doCloseFile(_model, index);
}

bool IDEWindow :: closeAll()
{
   for (int index = 1; index <= (int)_model->sourceViewModel.getDocumentCount(); index++) {
      if (!saveBeforeClose(index))
         return false;
   }

   return _controller->doCloseAll(_model, false);
}

void IDEWindow :: closeAllButActive()
{
   int currentIndex = _model->sourceViewModel.getCurrentIndex();
   for (int index = 1; index <= (int)_model->sourceViewModel.getDocumentCount(); index++) {
      if (index != currentIndex && !saveBeforeClose(index))
         return;
   }

   _controller->doCloseAllButActive(_model);
}

void IDEWindow :: openProject()
{
   PathString path;
   if (projectDialog.openFile(path)) {
      closeProject();

      _controller->doOpenProject(_model, *path);
      _recentProjectList.reload();
   }
}

void IDEWindow :: openProject(path_t path)
{
   closeProject();

   _controller->doOpenProject(_model, path);
   _recentProjectList.reload();
}

bool IDEWindow :: closeProject()
{
   if (_model->projectModel.empty)
      return true;

   if (_controller->ifProjectNotSaved(_model)) {
      auto result = messageDialog.question(QUESTION_SAVEPROJECT_CHANGES);
      if (result == MessageDialogBase::Answer::Cancel) {
         return false;
      }
      else if (result == MessageDialogBase::Answer::Yes) {
         saveProject(false);
      }
   }

   for (int index = 1; index <= (int)_model->sourceViewModel.getDocumentCount(); index++) {
      if (!saveBeforeClose(index))
         return false;
   }

   return _controller->doCloseAll(_model, true);

   //_controller->onLayoutchange();
}

void IDEWindow :: selectWindow()
{
   auto retVal = windowDialog.selectWindow();
   switch (retVal.value2) {
      case WindowListDialogBase::Mode::Activate:
      {
         path_t path = _model->sourceViewModel.getDocumentPath(retVal.value1);
         _controller->doSelectWindow(_model->viewModel(), path);
         break;
      }
      case WindowListDialogBase::Mode::Close:
         closeFile(retVal.value1);
         break;
      default:
         break;
   }
}

void IDEWindow :: exit()
{
   bool successful = false;
   if (_model->projectModel.empty) {
      successful = closeProject();
   }
   else successful = closeAll();

   if(successful && _controller->onClose(_model)) {
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

void IDEWindow :: excludeFile()
{
   _controller->doExclude(_model);
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
         _children[_model->ideScheme.projectView]->show();
      }
      else _children[_model->ideScheme.projectView]->hide();

      onLayoutChange();
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
   updateCompileMenu(false, false, true);

   openResultTab(_model->ideScheme.compilerOutputControl);

   ((ControlBase*)_children[_model->ideScheme.compilerOutputControl])->clearValue();
}

void IDEWindow :: onCompilationEnd(int exitCode, int postponedAction)
{
   wchar_t* output = ((ControlBase*)_children[_model->ideScheme.compilerOutputControl])->getValue();
   ControlBase* messageLog = (ControlBase*)_children[_model->ideScheme.errorListControl];

   _controller->onCompilationCompletion(_model, exitCode, output, dynamic_cast<ErrorLogBase*>(messageLog));

   freestr(output);

   if (exitCode != EXIT_FAILURE) {
      switch ((DebugAction)postponedAction) {
         case DebugAction::Run:
            _controller->doDebugAction(_model, DebugAction::Run, messageDialog, true);
            break;
         case DebugAction::StepOver:
            _controller->doDebugAction(_model, DebugAction::StepOver, messageDialog, true);
            break;
         case DebugAction::StepInto:
            _controller->doDebugAction(_model, DebugAction::StepInto, messageDialog, true);
            break;
         case DebugAction::RunTo:
            _controller->doDebugAction(_model, DebugAction::RunTo, messageDialog, true);
            break;
         default:
            break;
      }
   }
}

void IDEWindow :: onErrorHighlight(int index)
{
   ErrorLogBase* resultBar = dynamic_cast<ErrorLogBase*>(_children[_model->ideScheme.errorListControl]);

   auto messageInfo = resultBar->getMessage(index);
   if (!messageInfo.path.empty())
      _controller->highlightError(_model, messageInfo.row, messageInfo.column, messageInfo.path);
}

void IDEWindow :: onDebugStep()
{
   MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);
   menu->enableMenuItemById(IDM_DEBUG_STOP, true);

   _controller->onDebuggerStep(_model);
}

void IDEWindow :: onDebugWatch()
{
   openResultTab(_model->ideScheme.debugWatch);

   ContextBrowserBase* contextBrowser = dynamic_cast<ContextBrowserBase*>(_children[_model->ideScheme.debugWatch]);

   _controller->refreshDebugContext(contextBrowser, _model);

   contextBrowser->expandRootNode();
}

void IDEWindow :: onDebugEnd()
{
   ContextBrowserBase* contextBrowser = dynamic_cast<ContextBrowserBase*>(_children[_model->ideScheme.debugWatch]);

   contextBrowser->clearRootNode();

   _controller->onDebuggerStop(_model);
}

void IDEWindow :: onDebugWatchBrowse(BrowseNMHDR* rec)
{
   ContextBrowserBase* contextBrowser = dynamic_cast<ContextBrowserBase*>(_children[_model->ideScheme.debugWatch]);

   if (rec->param) {
      _controller->refreshDebugContext(contextBrowser, _model, rec->item, rec->param);
   }
   else _controller->refreshDebugContext(contextBrowser, _model);
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

void IDEWindow :: updateCompileMenu(bool compileEnable, bool debugEnable, bool stopEnable)
{
   enableMenuItemById(IDM_PROJECT_COMPILE, compileEnable, false);
   enableMenuItemById(IDM_PROJECT_OPTION, compileEnable, false);

   enableMenuItemById(IDM_DEBUG_RUN, debugEnable, true);
   enableMenuItemById(IDM_DEBUG_STEPINTO, debugEnable, true);
   enableMenuItemById(IDM_DEBUG_STEPOVER, debugEnable, true);
   enableMenuItemById(IDM_DEBUG_RUNTO, debugEnable, false);

   enableMenuItemById(IDM_DEBUG_STOP, stopEnable, true);
}

void IDEWindow :: onProjectRefresh(bool empty)
{
   updateCompileMenu(!empty, !empty, false);

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
         saveFile(-1);
         break;
      case IDM_FILE_SAVEAS:
         saveFileAs(-1);
         break;
      case IDM_FILE_SAVEALL:
         saveAll();
         break;
      case IDM_FILE_SAVEPROJECT:
         saveProject(true);
         break;
      case IDM_FILE_CLOSE:
         closeFile(-1);
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
         if (_model->autoSave)
            saveAll();

         _controller->doCompileProject(_model);
         break;
      case IDM_PROJECT_INCLUDE:
         includeFile();
         break;
      case IDM_PROJECT_EXCLUDE:
         excludeFile();
         break;
      case IDM_PROJECT_OPTION:
         _controller->doChangeProject(projectSettingsDialog, _model);
         break;
      case IDM_DEBUG_RUN:
         if (_model->autoSave)
            saveAll();

         _controller->doDebugAction(_model, DebugAction::Run, messageDialog, false);
         break;
      case IDM_DEBUG_STEPOVER:
         if (_model->autoSave)
            saveAll();

         _controller->doDebugAction(_model, DebugAction::StepOver, messageDialog, false);
         break;
      case IDM_DEBUG_STEPINTO:
         if (_model->autoSave)
            saveAll();

         _controller->doDebugAction(_model, DebugAction::StepInto, messageDialog, false);
         break;
      case IDM_DEBUG_RUNTO:
         if (_model->autoSave)
            saveAll();

         _controller->doDebugAction(_model, DebugAction::RunTo, messageDialog, false);
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
      case IDM_DEBUG_SWITCHHEXVIEW:
         switchHexMode();
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
         openProject(*path);
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
      case IDM_EDITOR_FONT_OPTIONS:
         _controller->doConfigureFontSettings(fontSettingsDialog, _model);
         break;
      case IDM_IDE_OPTIONS:
         _controller->doConfigureIDESettings(ideSettingsDialog, _model);
         break;
      case IDM_DEBUGGER_OPTIONS:
         _controller->doConfigureDebuggerSettings(debuggerSettingsDialog, _model);
         break;
      case IDM_WINDOW_WINDOWS:
         selectWindow();         
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

void IDEWindow :: switchHexMode()
{
   _model->contextBrowserModel.hexadecimalMode = !_model->contextBrowserModel.hexadecimalMode;

   refreshDebugNode();
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

   menu->checkMenuItemById(IDM_DEBUG_SWITCHHEXVIEW, _model->contextBrowserModel.hexadecimalMode);

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

   enableMenuItemById(IDM_EDIT_PASTE, Clipboard::isAvailable(), true);

   menu->show(_handle, p);
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
      updateCompileMenu(false, true, true);
   }   
   else if (test(rec->status, STATUS_DEBUGGER_RUNNING)) {
      _model->status = IDEStatus::Running;

      onStatusBarChange();
      updateCompileMenu(false, false, true);
   }
   else if (test(rec->status, STATUS_DEBUGGER_FINISHED)) {
      _model->status = IDEStatus::Stopped;

      onStatusBarChange();
      updateCompileMenu(true, true, false);

      onDebugEnd();
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
      onDebugWatch();
      onDebugStep();
   }

   if (test(rec->status, STATUS_FRAME_ACTIVATE)) {
      onActivate();
   }
}

void IDEWindow :: onTextModelChange(TextViewModelNMHDR* rec)
{
   if (test(rec->status, STATUS_COLORSCHEME_CHANGED)) {
      onColorSchemeChange();
      rec->docStatus.frameChanged = true;
   }

   onDocumentUpdate(rec->docStatus);
   onIDEStatusChange(rec);
   if (test(rec->status, STATUS_FRAME_CHANGED)) {
      onDocumentSelection();
   }
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

void IDEWindow :: onDocumentSelection()
{
   MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);
   auto docInfo = _model->sourceViewModel.DocView();

   menu->enableMenuItemById(IDM_PROJECT_INCLUDE, docInfo && !docInfo->isIncluded());
   menu->enableMenuItemById(IDM_PROJECT_EXCLUDE, docInfo && docInfo->isIncluded());
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

      enableMenuItemById(IDM_PROJECT_INCLUDE, false, false);
      enableMenuItemById(IDM_PROJECT_EXCLUDE, false, false);
   }

   enableMenuItemById(IDM_EDIT_CUT, !empty, true);
   enableMenuItemById(IDM_EDIT_COPY, !empty, true);
   enableMenuItemById(IDM_EDIT_PASTE, !empty, true);

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

   //if (!empty)
   //   _children[_model->ideScheme.textFrameId]->refresh();

   invalidate();
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
         onCompilationEnd(((CompletionNMHDR*)hdr)->exitCode, ((CompletionNMHDR*)hdr)->postponedAction);
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
      case EVENT_BROWSE_CONTEXT:
         onDebugWatchBrowse((BrowseNMHDR*)hdr);
         break;
      case EVENT_TEXT_MARGINLICK:
         _controller->toggleBreakpoint(_model, -1);
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

void IDEWindow :: onDebuggerSourceNotFound()
{
   MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);
   menu->enableMenuItemById(IDM_DEBUG_STOP, true);

   _controller->onDebuggerNoSource(messageDialog, _model);
}

bool IDEWindow :: onClose()
{
   if (_model->projectModel.empty) {
      closeProject();
   }
   else closeAll();

   if (!_controller->onClose(_model))
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

      menu->enableMenuItemById(IDM_EDIT_COMMENT, isSelected);
      menu->enableMenuItemById(IDM_EDIT_UNCOMMENT, isSelected);
      menu->enableMenuItemById(IDM_EDIT_DELETE, isSelected);
   }
   if (changeStatus.textChanged) {
      MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);

      enableMenuItemById(IDM_EDIT_UNDO, docInfo ? docInfo->canUndo() : false, true);
      enableMenuItemById(IDM_EDIT_REDO, docInfo ? docInfo->canRedo() : false, true);
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

   _viewFactory->styleControl(_children[_model->ideScheme.projectView]);

   _viewFactory->styleControl(this);

   refresh();
}

void IDEWindow :: onDropFiles(HDROP hDrop)
{
   TCHAR szName[MAX_PATH];

   int count = DragQueryFile(hDrop, 0xFFFFFFFF, szName, MAX_PATH);
   for (int i = 0; i < count; i++)
   {
      DragQueryFile(hDrop, i, szName, MAX_PATH);

      if(PathUtil::checkExtension(path_t(szName), "l")) {
         _controller->doOpenFile(_model, path_t(szName));
      }
      else if (PathUtil::checkExtension(path_t(szName), "prj")) {
         openProject(path_t(szName));
         break;
      }
   }

   DragFinish(hDrop);
}
