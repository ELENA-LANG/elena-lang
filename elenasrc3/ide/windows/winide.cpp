//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "windows/winide.h"

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

IDEWindow :: IDEWindow(wstr_t title, IDEController* controller, IDEModel* model, HINSTANCE instance, int textFrameId) : 
   SDIWindow(title), 
   dialog(instance, 
      this, Dialog::SourceFilter, 
      OPEN_FILE_CAPTION, 
      *model->projectModel.paths.lastPath),
   clipboard(this)
{
   this->_instance = instance;
   this->_controller = controller;
   this->_model = model;
   this->_textFrameId = textFrameId;
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
   _controller->doOpenFile(dialog, _model);
}

void IDEWindow :: saveFile()
{
   _controller->doSaveFile(dialog, _model, false);
}

void IDEWindow::closeFile()
{
   _controller->doCloseFile(dialog, _model);
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

bool IDEWindow :: onCommand(int command)
{
   switch (command) {
      case IDM_FILE_NEW:
         newFile();
         break;
      case IDM_FILE_OPEN:
         openFile();
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
      case IDM_DEBUG_RUN:
         _controller->projectController.doDebugAction(_model->projectModel, DebugAction::Run);
         break;
      case IDM_DEBUG_STEPOVER:
         _controller->projectController.doDebugAction(_model->projectModel, DebugAction::StepOver);
         break;
      case IDM_DEBUG_STEPINTO:
         _controller->projectController.doDebugAction(_model->projectModel, DebugAction::StepInto);
         break;
      default:
         return false;
   }

   return true;
}

void IDEWindow :: onModelChange(ExtNMHDR* hdr)
{
   auto docView = _model->sourceViewModel.DocView();

   switch (hdr->extParam) {
      case NOTIFY_SOURCEMODEL:
         docView->notifyOnChange();
         break;
      case NOTIFY_CURRENTVIEW_CHANGED:
         _model->sourceViewModel.afterDocumentSelect(hdr->extParam);
         _model->sourceViewModel.onModelChanged();
         break;
      case NOTIFY_CURRENTVIEW_SHOW:
         _children[_textFrameId]->show();
         break;
      case NOTIFY_CURRENTVIEW_HIDE:
         _children[_textFrameId]->hide();
         break;
      default:
         break;
   }   
}

void IDEWindow :: onTabSelChanged(HWND wnd)
{
   for (size_t i = 0; i < _childCounter; i++) {
      if (_children[i]->checkHandle(wnd)) {
         _children[i]->onSelChanged();
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
      default:
         break;
   }
}
