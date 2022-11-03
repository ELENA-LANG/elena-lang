//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "windows/winide.h"
#include "windows/wintabbar.h"
#include "windows/wintreeview.h"

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

void Clipboard :: pasteFromClipboard(DocumentView* docView)
{
   if (begin()) {
      HGLOBAL buffer = get();
      wchar_t* text = allocateBuffer(buffer);
      if (!emptystr(text)) {
         docView->insertLine(text, getlength(text));

         freeBuffer(buffer);
      }

      end();
   }
}

// --- IDEWindow ---

IDEWindow :: IDEWindow(wstr_t title, IDEController* controller, IDEModel* model, HINSTANCE instance) : 
   SDIWindow(title), 
   fileDialog(instance,
      this, Dialog::SourceFilter, 
      OPEN_FILE_CAPTION, 
      *model->projectModel.paths.lastPath),
   projectDialog(instance,
      this, Dialog::ProjectFilter,
      OPEN_PROJECT_CAPTION,
      *model->projectModel.paths.lastPath),
   clipboard(this)
{
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

void IDEWindow :: openFile()
{
   _controller->doOpenFile(fileDialog, _model);
}

void IDEWindow :: saveFile()
{
   _controller->doSaveFile(fileDialog, _model, false, true);
}

void IDEWindow::closeFile()
{
   _controller->doCloseFile(fileDialog, _model);
}

void IDEWindow :: openProject()
{
   _controller->doOpenProject(projectDialog, _model);
}

void IDEWindow :: exit()
{
   if(_controller->doExit()) {
      close();
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
   _model->sourceViewModel.onModelChanged();
}

void IDEWindow :: openResultTab(int controlIndex)
{
   TabBar* resultBar = (TabBar*)_children[_model->ideScheme.resultControl];

   if(!resultBar->selectTabChild((ControlBase*)_children[controlIndex])) {
      resultBar->addTabChild(_model->ideScheme.captions.get(controlIndex), (ControlBase*)_children[controlIndex]);
      resultBar->selectTabChild((ControlBase*)_children[controlIndex]);
   }

   resultBar->show();

   refresh();
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

   _controller->highlightError(_model, messageInfo.row, messageInfo.column, messageInfo.path);
}

void IDEWindow :: onProjectViewSel(size_t index)
{
   _controller->openProjectSourceByIndex(_model, index);
}

void IDEWindow :: onProjectChange()
{
   TreeView* projectTree = dynamic_cast<TreeView*>(_children[_model->ideScheme.projectView]);

   projectTree->clear(nullptr);
   TreeViewItem root = projectTree->insertTo(nullptr, *_model->projectModel.name, -1, true);

   PathString diskCriteria(":\\");

   int srcIndex = 0;
   wchar_t buffer[IDENTIFIER_LEN + 1];
   for(auto it = _model->projectModel.sources.start(); !it.eof(); ++it) {
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

bool IDEWindow :: onCommand(int command)
{
   switch (command) {
      case IDM_FILE_NEW:
         newFile();
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
      case IDM_FILE_CLOSE:
         closeFile();
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
      case IDM_PROJECT_COMPILE:
         _controller->doCompileProject(projectDialog, _model);
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
      default:
         return false;
   }

   return true;
}

void IDEWindow :: onModelChange(ExtNMHDR* hdr)
{
   auto docView = _model->sourceViewModel.DocView();

   switch (hdr->extParam1) {
      case NOTIFY_SOURCEMODEL:
         docView->notifyOnChange();
         break;
      case NOTIFY_PROJECTMODEL:
         onProjectChange();
         break;
      case NOTIFY_CURRENTVIEW_CHANGED:
         _model->sourceViewModel.afterDocumentSelect(hdr->extParam1);
         _model->sourceViewModel.onModelChanged();
         break;
      case NOTIFY_CURRENTVIEW_SHOW:
         _children[_model->ideScheme.textFrameId]->show();
         break;
      case NOTIFY_CURRENTVIEW_HIDE:
         _children[_model->ideScheme.textFrameId]->hide();
         break;
      case NOTIFY_LAYOUT_CHANGED:
         onResize();
         break;
      default:
         break;
   }   
}

void IDEWindow :: onNotifyMessage(ExtNMHDR* hdr)
{
   auto docView = _model->sourceViewModel.DocView();

   switch (hdr->extParam1) {
      case NOTIFY_SHOW_RESULT:
         openResultTab(hdr->extParam2);
         break;
      case NOTIFY_ACTIVATE_EDITFRAME:
         setChildFocus(_model->ideScheme.textFrameId);
         break;
      case NOTIFY_LAYOUT_CHANGED:
         onResize();
         break;
      case NOTIFY_COMPILATION_RESULT:
         onCompilationEnd(hdr->extParam2);
         break;
      case NOTIFY_ERROR_HIGHLIGHT_ROW:
         onErrorHighlight(hdr->extParam2);
         break;
      case NOTIFY_START_COMPILATION:
         onComilationStart();
         break;
      case NOTIFY_PROJECTVIEW_SEL:
         onProjectViewSel(hdr->extParam2);
         break;
      default:
         break;
   }
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

void IDEWindow :: onDoubleClick(NMHDR* hdr)
{
   for (size_t i = 0; i < _childCounter; i++) {
      if (_children[i]->checkHandle(hdr->hwndFrom)) {
         ((ControlBase*)_children[i])->onDoubleClick(hdr);
         break;
      }
   }
}

void IDEWindow :: onNotify(NMHDR* hdr)
{
   switch (hdr->code) {
      case NMHDR_Model:
         onModelChange((ExtNMHDR*)hdr);
         break;
      case TCN_SELCHANGE:
         onTabSelChanged(hdr->hwndFrom);
         break;
      case NM_DBLCLK:
         onDoubleClick(hdr);
         break;
      case NMHDR_Message:
         onNotifyMessage((ExtNMHDR*)hdr);
         break;
      case TVN_SELCHANGED:
         onTreeSelChanged(hdr->hwndFrom);
         break;
      default:
         break;
   }
}
