//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Implementation File
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <tchar.h>

#include "windows/winide.h"

#include "eng/messages.h"

#include "windows/wintreeview.h"
#include "windows/wintabbar.h"
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

// --- IDEWindow ---

IDEWindow :: IDEWindow(wstr_t title, IDEController* controller, IDEModel* model, HINSTANCE instance) : 
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
   _windowList(controller, model->viewModel())
{
   this->_instance = instance;
   this->_controller = controller;
   this->_model = model;

   model->sourceViewModel.attachDocListener(this);
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

void IDEWindow :: openProject()
{
   _controller->doOpenProject(projectDialog, messageDialog, _model);
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

      onLayoutChange(IDE_LAYOUT_CHANGED);
   }
}

void IDEWindow :: closeResultTab(int controlIndex)
{
   _children[controlIndex]->hide();

   TabBar* resultBar = (TabBar*)_children[_model->ideScheme.resultControl];

   resultBar->removeTabChild((ControlBase*)_children[controlIndex]);

   if (resultBar->empty()) {
      resultBar->hide();
   }
   else resultBar->selectTab(0);

   onLayoutChange(IDE_LAYOUT_CHANGED);
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

void IDEWindow :: onProjectViewSel(int index)
{
   _controller->doOpenProjectSourceByIndex(_model, index);

   _children[_model->ideScheme.textFrameId]->setFocus();
}

void IDEWindow :: onDebugWatch()
{
   openResultTab(_model->ideScheme.debugWatch);

   ContextBrowserBase* contextBrowser = dynamic_cast<ContextBrowserBase*>(_children[_model->ideScheme.debugWatch]);

   _controller->refreshDebugContext(contextBrowser, _model);

   contextBrowser->expandRootNode();
}

void IDEWindow :: onDebugWatchBrowse(size_t item, size_t param)
{
   if (param) {
      ContextBrowserBase* contextBrowser = dynamic_cast<ContextBrowserBase*>(_children[_model->ideScheme.debugWatch]);
      _controller->refreshDebugContext(contextBrowser, _model, item, param);
   }
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

   MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);
   menu->enableMenuItemById(IDM_PROJECT_CLOSE, !empty);

   menu->enableMenuItemById(IDM_PROJECT_COMPILE, !empty);
   menu->enableMenuItemById(IDM_DEBUG_RUN, !empty);
   menu->enableMenuItemById(IDM_DEBUG_STEPINTO, !empty);
   menu->enableMenuItemById(IDM_DEBUG_STEPOVER, !empty);
   menu->enableMenuItemById(IDM_DEBUG_RUNTO, !empty);
   menu->enableMenuItemById(IDM_DEBUG_STOP, !empty);
}

void IDEWindow :: onLayoutChange(NotificationStatus status)
{
   bool empty = _model->sourceViewModel.getDocumentCount() == 0;

   if (test(status, FRAME_VISIBILITY_CHANGED)) {
      if (!empty) {
         _children[_model->ideScheme.textFrameId]->show();
         _children[_model->ideScheme.textFrameId]->setFocus();
      }
      else _children[_model->ideScheme.textFrameId]->hide();
   }
   if (test(status, PROJECT_CHANGED)) {
      onProjectChange(_model->projectModel.empty);
   }

   MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);
   menu->checkMenuItemById(IDM_VIEW_OUTPUT, _children[_model->ideScheme.compilerOutputControl]->visible());
   menu->checkMenuItemById(IDM_VIEW_PROJECTVIEW, _children[_model->ideScheme.projectView]->visible());
   menu->checkMenuItemById(IDM_VIEW_MESSAGES, _children[_model->ideScheme.errorListControl]->visible());
   menu->checkMenuItemById(IDM_VIEW_WATCH, _children[_model->ideScheme.debugWatch]->visible());
   menu->checkMenuItemById(IDM_VIEW_VMCONSOLE, _children[_model->ideScheme.vmConsoleControl]->visible());

   menu->enableMenuItemById(IDM_FILE_SAVE, !empty);
   menu->enableMenuItemById(IDM_FILE_CLOSE, !empty);

   if (empty) {
      menu->enableMenuItemById(IDM_EDIT_UNDO, false);
      menu->enableMenuItemById(IDM_EDIT_REDO, false);
      
      menu->enableMenuItemById(IDM_EDIT_CUT, false);
      menu->enableMenuItemById(IDM_EDIT_COPY, false);
      menu->enableMenuItemById(IDM_EDIT_PASTE, false);
      menu->enableMenuItemById(IDM_EDIT_DELETE, false);
      menu->enableMenuItemById(IDM_EDIT_COMMENT, false);
      menu->enableMenuItemById(IDM_EDIT_UNCOMMENT, false);
   }
   else menu->enableMenuItemById(IDM_EDIT_PASTE, true);

   onResize();

   if (!empty)
      _children[_model->ideScheme.textFrameId]->refresh();
}

void IDEWindow :: onTextFrameChange(NotificationStatus status)
{
   DocumentChangeStatus changeStatus = { test(status, FRAME_CHANGED) };

   if (_model->sourceViewModel.DocView())
      _model->sourceViewModel.DocView()->notifyOnChange(changeStatus);
}

void IDEWindow :: onIDEChange(NotificationStatus status)
{
   if (test(status, IDE_LAYOUT_CHANGED)) {
      onLayoutChange(status);
   }

   if (test(status, FRAME_CHANGED)) {
      onTextFrameChange(status);
   }

   if (test(status, IDE_STATUS_CHANGED)) {
      _children[_model->ideScheme.statusBar]->refresh();
   }

   if (test(status, IDE_COMPILATION_STARTED))
      onComilationStart();

   if (test(status, FRAME_ACTIVATE)) {
      onActivate();
   }

   //onIDEViewUpdate(true);
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
      case IDM_EDIT_INDENT:
         _controller->doIndent(_model);
         break;
      case IDM_EDIT_OUTDENT:
         _controller->doOutdent(_model);
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

void IDEWindow :: onStatusChange(StatusNMHDR* rec)
{  
   switch (rec->code) {
      case NOTIFY_ONSTART:
         _controller->init(_model);
         break;
      case NOTIFY_IDE_CHANGE:
         onIDEChange(rec->status);
         break;
      case NOTIFY_DEBUG_START:
         onDebuggerStart();
         break;
      case NOTIFY_DEBUG_LOAD:
         onDebuggerHook();
         break;
      case NOTIFY_DEBUG_CHANGE:
         onDebuggerUpdate(rec);
         break;
      default:
         break;
   }
}

//void IDEWindow :: onModelChange(ExtNMHDR* hdr)
//{
   //auto docView = _model->sourceViewModel.DocView();

   //switch (hdr->extParam1) {
   //   case NOTIFY_ONSTART:
   //      
   //      break;
   //   case NOTIFY_SOURCEMODEL:
   //      docView->notifyOnChange();
   //      break;
   //   case NOTIFY_DEBUGWATCH:
   //      onDebugWatch();
   //      break;
   //   case NOTIFY_CURRENTVIEW_SHOW:
   //      _children[_model->ideScheme.textFrameId]->show();
   //      onResize();
   //      break;
   //   case NOTIFY_CURRENTVIEW_HIDE:
   //      _children[_model->ideScheme.textFrameId]->hide();
   //      onResize();
   //      break;
   //   case NOTIFY_LAYOUT_CHANGED:
   //      onLayoutChange();
   //      break;
   //   default:
   //      break;
   //}   
//}

//void IDEWindow :: onNotifyMessage(ExtNMHDR* hdr)
//{
   //auto docView = _model->sourceViewModel.DocView();

   //switch (hdr->extParam1) {
   //   case NOTIFY_ACTIVATE_EDITFRAME:
   //      setChildFocus(_model->ideScheme.textFrameId);
   //      break;
   //   case NOTIFY_LAYOUT_CHANGED:
   //      onLayoutChange();
   //      break;
   //   case NOTIFY_REFRESH:
   //      
   //      break;
   //   default:
   //      break;
   //}
//}

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

void IDEWindow :: onSelection(SelectionNMHDR* rec)
{
   switch (rec->code) {
      case NOTIFY_PROJECTVIEW_SEL:
         onProjectViewSel(rec->param);
         break;
      case NOTIFY_TEXTFRAME_SEL:
         onTextFrameChange(FRAME_CHANGED);
         break;
      case NOTIFY_SHOW_RESULT:
         openResultTab((int)rec->param);
         break;
      case NOTIFY_ERROR_SEL:
         onErrorHighlight((int)rec->param);
         break;
      case NOTIFY_REFRESH:
         onChildRefresh((int)rec->param);
         break;
      default:
         break;
   }
}

void IDEWindow :: onTreeItem(TreeItemNMHDR* rec)
{
   switch (rec->code) {
      case NOTIFY_DEBUG_CONTEXT_EXPANDED:
         onDebugWatchBrowse(rec->item, rec->param);
         break;
      default:
         break;
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

void IDEWindow :: onComplition(CompletionNMHDR* rec)
{
   switch (rec->code) {
      case NOTIFY_COMPILATION_RESULT:
         onCompilationEnd(rec->param);
         break;
      case NOTIFY_DEBUGGER_RESULT:
         onDebugResult(rec->param);
         break;
      default:
         break;
   }
}

void IDEWindow :: onChildRefresh(int controlId)
{
   _children[controlId]->refresh();
}

void IDEWindow :: onNotify(NMHDR* hdr)
{
   switch (hdr->code) {
      case STATUS_NOTIFICATION:
         onStatusChange((StatusNMHDR*)hdr);
         break;
      case STATUS_SELECTION:
         onSelection((SelectionNMHDR*)hdr);
         break;
      case STATUS_TREEITEM:
         onTreeItem((TreeItemNMHDR*)hdr);
         break;
      case STATUS_COMPLETION:
         onComplition((CompletionNMHDR*)hdr);
         break;
      case CONTEXT_MENU_ON:
         onContextMenu((ContextMenuNMHDR*)hdr);
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

void IDEWindow :: onDebuggerUpdate(StatusNMHDR* rec)
{
   MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);

   menu->enableMenuItemById(IDM_DEBUG_STOP, true);

   if (test(rec->status, DEBUGWATCH_CHANGED)) {
      onDebugWatch();
   }
   if (test(rec->status, FRAME_CHANGED)) {
      onTextFrameChange(rec->status);
   }
}

//void IDEWindow :: onIDEViewUpdate(bool forced)
//{
//   auto doc = _model->viewModel()->DocView();
//   MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);
//
//   IDESttus status =
//   {
//      doc != nullptr,
//      !_model->projectModel.empty,
//      _controller->projectController.isStarted()
//   };
//
//   if (doc != nullptr) {
//      status.canUndo = doc->canUndo();
//      status.canRedo = doc->canRedo();
//      status.hasSelection = doc->hasSelection();
//   }
//
//   if (forced || status.hasDocument != _ideStatus.hasDocument) {
//      menu->enableMenuItemById(IDM_FILE_SAVE, status.hasDocument);
//      menu->enableMenuItemById(IDM_FILE_CLOSE, status.hasDocument);
//
//      if (!status.hasDocument) {
//         menu->enableMenuItemById(IDM_EDIT_UNDO, status.hasDocument);
//         menu->enableMenuItemById(IDM_EDIT_REDO, status.hasDocument);
//
//         menu->enableMenuItemById(IDM_EDIT_CUT, status.hasDocument);
//         menu->enableMenuItemById(IDM_EDIT_COPY, status.hasDocument);
//         menu->enableMenuItemById(IDM_EDIT_DELETE, status.hasDocument);
//         menu->enableMenuItemById(IDM_EDIT_COMMENT, status.hasDocument);
//      }
//
//      menu->enableMenuItemById(IDM_EDIT_PASTE, status.hasDocument);
//   }
//
//   if (forced || status.hasProject != _ideStatus.hasProject) {
//      menu->enableMenuItemById(IDM_PROJECT_COMPILE, status.hasProject);
//      menu->enableMenuItemById(IDM_DEBUG_RUN, status.hasProject);
//      menu->enableMenuItemById(IDM_DEBUG_STEPINTO, status.hasProject);
//      menu->enableMenuItemById(IDM_DEBUG_STEPOVER, status.hasProject);
//      menu->enableMenuItemById(IDM_DEBUG_RUNTO, status.hasProject);
//   }
//
//   if (forced || status.running != _ideStatus.running) {
//      onDebuggerUpdate(status.running);
//   }
//
//   _ideStatus = status;
//}

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

void IDEWindow :: onDocumentUpdate(DocumentChangeStatus& changeStatus)
{
   auto docInfo = _model->sourceViewModel.DocView();

   if (changeStatus.modifiedChanged) {
      ((TextViewFrame*)_children[_model->ideScheme.textFrameId])->onDocumentModeChanged(-1, docInfo->isModified());
   }

   if (changeStatus.selelectionChanged) {
      MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);

      bool isSelected = docInfo->hasSelection();

      menu->enableMenuItemById(IDM_EDIT_COPY, isSelected);
      menu->enableMenuItemById(IDM_EDIT_CUT, isSelected);
      menu->enableMenuItemById(IDM_EDIT_COMMENT, isSelected);
      menu->enableMenuItemById(IDM_EDIT_UNCOMMENT, isSelected);
      menu->enableMenuItemById(IDM_EDIT_DELETE, isSelected);
   }
   if (changeStatus.textChanged) {
      MenuBase* menu = dynamic_cast<MenuBase*>(_children[_model->ideScheme.menu]);

      menu->enableMenuItemById(IDM_EDIT_UNDO, docInfo->canUndo());
      menu->enableMenuItemById(IDM_EDIT_REDO, docInfo->canRedo());
   }

   if (changeStatus.textChanged || changeStatus.caretChanged) {
      _controller->onStatusChange(_model, IDEStatus::Ready);
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
}
