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

void TextViewController :: newDocument(TextViewModelBase* model, ustr_t name)
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

   model->selectDocumentView(currentIndex);
}

void TextViewController :: selectPreviousDocument(TextViewModelBase* model)
{
   int currentIndex = model->getDocumentIndex(model->getDocumentName(-1));
   if (currentIndex == 1) {
      currentIndex = model->getDocumentCount();
   }
   else currentIndex--;

   model->selectDocumentView(currentIndex);
}

bool TextViewController :: selectDocument(TextViewModelBase* model, int index, NotificationStatus& status)
{
   if (model->selectDocumentView(index)) {
      status |= FRAME_CHANGED;

      return true;
   }
   return false;
}

void TextViewController :: closeDocument(TextViewModelBase* model, int index, NotificationStatus& status)
{
   if (model->closeDocumentView(index))
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

   notifyOnChange(model, status);
}

void TextViewController :: undo(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->undo(status);

   notifyOnChange(model, status);
}

void TextViewController :: redo(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->redo(status);

   notifyOnChange(model, status);
}

bool TextViewController :: copyToClipboard(TextViewModelBase* model, ClipboardBase* clipboard)
{
   auto docView = model->DocView();
   if (docView->hasSelection()) {
      if (clipboard->copyToClipboard(docView)) {
         notifyOnClipboardOperation(clipboard);

         return true;
      }
   }
   return false;
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

      notifyOnChange(model, status);
   }
}

bool TextViewController :: insertNewLine(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->eraseSelection(status);
      docView->insertNewLine(status);

      notifyOnChange(model, status);

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

      notifyOnChange(model, status);

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

      notifyOnChange(model, status);

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

   notifyOnChange(model, status);
}

void TextViewController :: insertBlockText(TextViewModelBase* model, const_text_t s, size_t length)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->blockInserting(status, s, length);
   }

   notifyOnChange(model, status);
}

void TextViewController :: deleteBlockText(TextViewModelBase* model, const_text_t s, size_t length)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->blockDeleting(status, s, length);
   }

   notifyOnChange(model, status);
}

void TextViewController :: moveCaretDown(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveFrameDown(status);
   }
   else docView->moveDown(status, kbShift);

   notifyOnChange(model, status);
}

void TextViewController :: moveCaretLeft(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveLeftToken(status, kbShift);
   }
   else docView->moveLeft(status, kbShift);

   notifyOnChange(model, status);
}

void TextViewController :: selectWord(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->moveLeftToken(status, false);
   docView->moveRightToken(status, true, true);

   notifyOnChange(model, status);
}

void TextViewController::selectAll(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->moveFirst(status, false);
   docView->moveEnd(status, true);

   notifyOnChange(model, status);
}

void TextViewController :: notifyOnChange(TextViewModelBase* model, DocumentChangeStatus& changeStatus)
{
   model->notifyOnChange(changeStatus);
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

   notifyOnChange(model, status);
}

void TextViewController :: moveCaretUp(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (!kbCtrl) {
      docView->moveUp(status, kbShift);
   }
   else docView->moveFrameUp(status);

   notifyOnChange(model, status);
}

void TextViewController :: moveCaretHome(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveFirst(status, kbShift);
   }
   else docView->moveHome(status, kbShift);

   notifyOnChange(model, status);
}

void TextViewController :: moveCaretEnd(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveEnd(status, kbShift);
   }
   else docView->moveLast(status, kbShift);

   notifyOnChange(model, status);
}

void TextViewController :: movePageDown(TextViewModelBase* model, bool kbShift)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->movePageDown(status, kbShift);

   notifyOnChange(model, status);
}

void TextViewController :: movePageUp(TextViewModelBase* model, bool kbShift)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->movePageUp(status, kbShift);

   notifyOnChange(model, status);
}

void TextViewController :: moveToFrame(TextViewModelBase* model, int col, int row, bool kbShift)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->moveToFrame(status, col, row, kbShift);

   notifyOnChange(model, status);
}

void TextViewController :: resizeModel(TextViewModelBase* model, Point size)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if(docView->getSize() != size) {
      model->resize(size);

      status.frameChanged = true;
   }

   notifyOnChange(model, status);
}

bool TextViewController :: findText(TextViewModelBase* model, FindModel* findModel)
{
   DocumentChangeStatus docStatus = {};

   auto docView = model->DocView();
   if (docView && docView->findLine(docStatus, findModel->text.str(), findModel->matchCase, 
      findModel->wholeWord)) 
   {
      notifyOnChange(model, docStatus);

      return true;
   }

   return false;
}

bool TextViewController :: replaceText(TextViewModelBase* model, FindModel* findModel)
{
   DocumentChangeStatus docStatus = {};

   auto docView = model->DocView();
   if (docView) {
      docView->insertLine(docStatus, findModel->newText.str(), findModel->newText.length());
      notifyOnChange(model, docStatus);

      return true;
   }

   return false;
}

void TextViewController :: goToLine(TextViewModelBase* model, int row)
{
   DocumentChangeStatus docStatus = {};

   auto docView = model->DocView();
   if (docView) {
      Point caret = docView->getCaret();

      caret.y = row - 1;

      docView->setCaret(caret, false, docStatus);
      notifyOnChange(model, docStatus);
   }
}
