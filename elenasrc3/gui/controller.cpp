//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI Controller implementation File
//                                             (C)2021-2022, by Aleksey Rakov
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
   auto docView = model->DocView();

   if (_settings.tabUsing) {
      docView->tabbing('\t', 1, true);
   }
   else {
      if (!docView->hasSelection()) {
         int shift = calcTabShift(docView->getCaret().x, _settings.tabSize);
         docView->insertChar(' ', shift);
      }
      else docView->tabbing(' ', _settings.tabSize, true);
   }
}

void TextViewController :: undo(TextViewModelBase* model)
{
   auto docView = model->DocView();

   docView->undo();
}

void TextViewController :: redo(TextViewModelBase* model)
{
   auto docView = model->DocView();

   docView->redo();
}

bool TextViewController :: copyToClipboard(TextViewModelBase* model, ClipboardBase* clipboard)
{
   auto docView = model->DocView();
   if (docView->hasSelection()) {
      return clipboard->copyToClipboard(docView);
   }
   else return false;
}

void TextViewController :: onTextChanged(TextViewModelBase* model, DocumentView* view)
{
   //if (view->status.isModeChanged()) {
   //   //model->onModelModeChanged(-1);
   //}
}

void TextViewController :: pasteFromClipboard(TextViewModelBase* model, ClipboardBase* clipboard)
{
   auto docView = model->DocView();
   if (!docView->status.readOnly) {
      clipboard->pasteFromClipboard(docView);

      onTextChanged(model, docView);
   }
}

bool TextViewController :: insertNewLine(TextViewModelBase* model)
{
   auto docView = model->DocView();
   if (!docView->status.readOnly) {
      docView->eraseSelection();
      docView->insertNewLine();

      onTextChanged(model, docView);

      return true;
   }

   return false;
}

bool TextViewController :: insertChar(TextViewModelBase* model, text_c ch)
{
   auto docView = model->DocView();
   if (!docView->status.readOnly && ch >= 0x20) {
      docView->eraseSelection();
      docView->insertChar(ch);

      onTextChanged(model, docView);

      return true;
   }

   return false;
}

bool TextViewController :: eraseChar(TextViewModelBase* model, bool moveback)
{
   auto docView = model->DocView();
   if (!docView->status.readOnly) {
      if (docView->hasSelection()) {
         docView->eraseSelection();
      }
      else docView->eraseChar(moveback);

      onTextChanged(model, docView);

      return true;
   }

   return false;
}

void TextViewController :: deleteText(TextViewModelBase* model)
{
   auto docView = model->DocView();
   if (!docView->status.readOnly) {
      docView->eraseSelection();

      onTextChanged(model, docView);
   }
}

void TextViewController :: insertBlockText(TextViewModelBase* model, const text_t s, size_t length)
{
   auto docView = model->DocView();
   if (!docView->status.readOnly) {
      docView->blockInserting(s, length);

      onTextChanged(model, docView);
   }
}

void TextViewController :: moveCaretDown(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   //auto docView = model->DocView();

   //if (kbCtrl) {
   //   docView->moveFrameDown();
   //}
   //else docView->moveDown(kbShift);

   //model->onModelChanged();
}

void TextViewController :: moveCaretLeft(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   //auto docView = model->DocView();

   //if (kbCtrl) {
   //   docView->moveLeftToken(kbShift);
   //}
   //else docView->moveLeft(kbShift);

   //model->onModelChanged();
}

void TextViewController :: selectWord(TextViewModelBase* model)
{
   auto docView = model->DocView();

   docView->moveLeftToken(false);
   docView->moveRightToken(true, true);
}

void TextViewController :: moveCaretRight(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   //auto docView = model->DocView();

   //if (kbCtrl) {
   //   docView->moveRightToken(kbShift);
   //}
   //else docView->moveRight(kbShift);

   //model->onModelChanged();
}

void TextViewController :: moveCaretUp(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   //auto docView = model->DocView();

   //if (!kbCtrl) {
   //   docView->moveUp(kbShift);
   //}
   //else docView->moveFrameUp();

   //model->onModelChanged();
}

void TextViewController :: moveCaretHome(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   //auto docView = model->DocView();

   //if (kbCtrl) {
   //   docView->moveFirst(kbShift);
   //}
   //else docView->moveHome(kbShift);

   //model->onModelChanged();
}

void TextViewController :: moveCaretEnd(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   //auto docView = model->DocView();

   //if (kbCtrl) {
   //   docView->moveEnd(kbShift);
   //}
   //else docView->moveLast(kbShift);

   //model->onModelChanged();
}

void TextViewController :: movePageDown(TextViewModelBase* model, bool kbShift)
{
   //auto docView = model->DocView();
   //docView->movePageDown(kbShift);

   //model->onModelChanged();
}

void TextViewController :: movePageUp(TextViewModelBase* model, bool kbShift)
{
   //auto docView = model->DocView();
   //docView->movePageUp(kbShift);

   //model->onModelChanged();
}

void TextViewController :: moveToFrame(TextViewModelBase* model, int col, int row, bool kbShift)
{
   //model->DocView()->moveToFrame(col, row, kbShift);

   //model->onModelChanged();
}
