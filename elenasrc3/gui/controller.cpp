//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI Controller implementation File
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "controller.h"
#include "elena.h"
#include "idecommon.h"

using namespace elena_lang;

// --- TextViewController ---

//void TextViewController :: onFrameChange()
//{
//   
//}

void TextViewController :: newDocument(TextViewModelBase* model, ustr_t name, int notifyMessage)
{
   Text* text = new Text(_settings.eolMode);
   text->create();

   model->addDocumentView(name, text, nullptr);
}

bool TextViewController :: openDocument(TextViewModelBase* model, ustr_t name, path_t path, 
   FileEncoding encoding)
{
   Text* text = new Text(_settings.eolMode);
   if (!text->load(path, encoding, false))
      return false;

   model->addDocumentView(name, text, path);

   return true;
}

void TextViewController :: selectNextDocument(TextViewModelBase* model)
{
   int currentIndex = model->getDocumentIndex(model->getDocumentName(-1));
   if (currentIndex == model->getDocumentCount()) {
      currentIndex = 1;
   }
   else currentIndex++;

   model->selectDocumentViewByIndex(currentIndex);
}

void TextViewController :: selectPreviousDocument(TextViewModelBase* model)
{
   int currentIndex = model->getDocumentIndex(model->getDocumentName(-1));
   if (currentIndex == 1) {
      currentIndex = model->getDocumentCount();
   }
   else currentIndex--;

   model->selectDocumentViewByIndex(currentIndex);
}

bool TextViewController :: selectDocument(TextViewModelBase* model, ustr_t name, NotificationStatus& status)
{
   if (model->selectDocumentView(name)) {
      status |= FRAME_CHANGED;

      return true;
   }
   return false;
}

void TextViewController :: closeDocument(TextViewModelBase* model, ustr_t name, NotificationStatus& status)
{
   if (model->closeDocumentView(name))
      status |= FRAME_CHANGED;
}

void TextViewController :: indent(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (_settings.tabUsing) {
      docView->tabbing(status, '\t', 1, true);
   }
   else {
      if (!docView->hasSelection()) {
         int shift = calcTabShift(docView->getCaret().x, _settings.tabSize);
         docView->insertChar(status, ' ', shift);
      }
      else docView->tabbing(status, ' ', _settings.tabSize, true);
   }

   notifyOnChange(docView, status);
}

void TextViewController :: undo(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->undo(status);

   notifyOnChange(docView, status);
}

void TextViewController :: redo(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->redo(status);

   notifyOnChange(docView, status);
}

bool TextViewController :: copyToClipboard(TextViewModelBase* model, ClipboardBase* clipboard)
{
   auto docView = model->DocView();
   if (docView->hasSelection()) {
      if (clipboard->copyToClipboard(docView)) {
         notifyOnClipboardOperation(clipboard);
      }
   }
   else return false;
}

//void TextViewController :: onTextChanged(TextViewModelBase* model, DocumentView* view)
//{
   //if (view->status.isModeChanged()) {
   //   //model->onModelModeChanged(-1);
   //}
//}

void TextViewController :: pasteFromClipboard(TextViewModelBase* model, ClipboardBase* clipboard)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      clipboard->pasteFromClipboard(status, docView);

      notifyOnChange(docView, status);
   }
}

bool TextViewController :: insertNewLine(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->eraseSelection(status);
      docView->insertNewLine(status);

      notifyOnChange(docView, status);

      return true;
   }

   return false;
}

bool TextViewController :: insertChar(TextViewModelBase* model, text_c ch)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly() && ch >= 0x20) {
      docView->eraseSelection(status);
      docView->insertChar(status, ch);

      notifyOnChange(docView, status);

      return true;
   }

   return false;
}

bool TextViewController :: eraseChar(TextViewModelBase* model, bool moveback)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      if (docView->hasSelection()) {
         docView->eraseSelection(status);
      }
      else docView->eraseChar(status, moveback);

      notifyOnChange(docView, status);

      return true;
   }

   return false;
}

void TextViewController :: deleteText(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->eraseSelection(status);
   }

   notifyOnChange(docView, status);
}

void TextViewController :: insertBlockText(TextViewModelBase* model, const text_t s, size_t length)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->blockInserting(status, s, length);
   }

   notifyOnChange(docView, status);
}

void TextViewController :: moveCaretDown(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveFrameDown(status);
   }
   else docView->moveDown(status, kbShift);

   notifyOnChange(docView, status);
}

void TextViewController :: moveCaretLeft(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveLeftToken(status, kbShift);
   }
   else docView->moveLeft(status, kbShift);

   notifyOnChange(docView, status);
}

void TextViewController :: selectWord(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->moveLeftToken(status, false);
   docView->moveRightToken(status, true, true);

   notifyOnChange(docView, status);
}

void TextViewController :: notifyOnChange(DocumentView* docView, DocumentChangeStatus& changeStatus)
{
   docView->notifyOnChange(changeStatus);

   //NotificationStatus status = NONE_CHANGED;
   //if (changeStatus.)

   //_notifier->notify(NOTIFY_CURRENTVIEW_CHANGED, status);
}

void TextViewController :: notifyOnClipboardOperation(ClipboardBase* clipboard)
{
   
}

void TextViewController :: moveCaretRight(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveRightToken(status, kbShift);
   }
   else docView->moveRight(status, kbShift);

   notifyOnChange(docView, status);
}

void TextViewController :: moveCaretUp(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (!kbCtrl) {
      docView->moveUp(status, kbShift);
   }
   else docView->moveFrameUp(status);

   notifyOnChange(docView, status);
}

void TextViewController :: moveCaretHome(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveFirst(status, kbShift);
   }
   else docView->moveHome(status, kbShift);

   notifyOnChange(docView, status);
}

void TextViewController :: moveCaretEnd(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveEnd(status, kbShift);
   }
   else docView->moveLast(status, kbShift);

   notifyOnChange(docView, status);
}

void TextViewController :: movePageDown(TextViewModelBase* model, bool kbShift)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->movePageDown(status, kbShift);

   notifyOnChange(docView, status);
}

void TextViewController :: movePageUp(TextViewModelBase* model, bool kbShift)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->movePageUp(status, kbShift);

   notifyOnChange(docView, status);
}

void TextViewController :: moveToFrame(TextViewModelBase* model, int col, int row, bool kbShift)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->moveToFrame(status, col, row, kbShift);

   notifyOnChange(docView, status);
}

void TextViewController :: resizeModel(TextViewModelBase* model, Point size)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if(docView->getSize() != size) {
      model->resize(size);

      status.frameChanged = true;
   }

   notifyOnChange(docView, status);
}
