//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI Controller implementation File
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "controller.h"
#include "elena.h"
#include "idecommon.h"

#ifdef _MSC_VER

#include <tchar.h>

#endif

using namespace elena_lang;

constexpr auto OPENING_BRACKETS = _T("({[\"");
constexpr auto CLOSING_BRACKETS  = _T(")}]\"");

bool isPairedBracket(text_c ch)
{
   size_t len = getlength(OPENING_BRACKETS);
   for (size_t i = 0; i < len; i++) {
      if (OPENING_BRACKETS[i] == ch)
         return true;
   }

   return false;
}

text_c getClosingBracket(text_c ch)
{
   size_t len = getlength(OPENING_BRACKETS);
   for (size_t i = 0; i < len; i++) {
      if (OPENING_BRACKETS[i] == ch)
         return CLOSING_BRACKETS[i];
   }

   return 0;
}

// --- TextViewController ---

void TextViewController :: newDocument(TextViewModelBase* model, ustr_t name, bool included)
{
   Text* text = new Text(model->settings.eolMode);
   text->create();

   model->addDocumentView(name, text, nullptr, included);
}

bool TextViewController :: openDocument(TextViewModelBase* model, ustr_t name, path_t path,
   FileEncoding encoding, bool included)
{
   Text* text = new Text(model->settings.eolMode);
   if (!text->load(path, encoding, false))
      return false;

   model->addDocumentView(name, text, path, included);

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

bool TextViewController :: selectDocument(TextViewModelBase* model, int index)
{
   return model->selectDocumentView(index);
}

void TextViewController :: closeDocument(TextViewModelBase* model, int index, int& status)
{
   if (model->closeDocumentView(index))
      status |= STATUS_FRAME_CHANGED;
}

void TextViewController :: indent(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (model->settings.tabUsing) {
      docView->tabbing(status, '\t', 1, true);
   }
   else {
      if (!docView->hasSelection()) {
         int shift = calcTabShift(docView->getCaret().x, model->settings.tabSize);
         docView->insertChar(status, ' ', shift);
      }
      else docView->tabbing(status, ' ', model->settings.tabSize, true);
   }

   notifyTextModelChange(model, status);
}

void TextViewController :: outdent(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (model->settings.tabUsing) {
      docView->tabbing(status, '\t', 1, false);
   }
   else {
      docView->tabbing(status, ' ', model->settings.tabSize, false);
   }

   notifyTextModelChange(model, status);
}

void TextViewController :: undo(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (docView->isReadOnly())
      return;

   docView->undo(status);

   notifyTextModelChange(model, status);
}

void TextViewController :: redo(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->redo(status);
   if (docView->isReadOnly())
      return;

   notifyTextModelChange(model, status);
}

bool TextViewController :: copyToClipboard(TextViewModelBase* model, ClipboardBase* clipboard)
{
   auto docView = model->DocView();

   if (clipboard->copyToClipboard(docView, docView->hasSelection())) {
      notifyOnClipboardOperation(clipboard);

      return true;
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

      notifyTextModelChange(model, status);
   }
}

void TextViewController :: pasteFromClipboard(TextViewModelBase* model, const_text_t text)
{
   if (emptystr(text))
      return;

   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      if (!emptystr(text)) {
         docView->insertLine(status, text, getlength(text));
      }

      notifyTextModelChange(model, status);
   }
}

bool TextViewController :: insertNewLine(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->eraseSelection(status);

      auto lastPoint = docView->getCaret();
      text_c currentChar = docView->getCurrentChar();
      docView->insertNewLine(status);
      if (currentChar == '}') {
         docView->setCaret(lastPoint, false, status);
         docView->insertNewLine(status);
      }

      notifyTextModelChange(model, status);

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

      if (isPairedBracket(ch)) {
         auto caret = docView->getCaret();
         if (caret.x == docView->getCurrentLineLength())
            docView->insertChar(status, getClosingBracket(ch), 1, false);
      }

      notifyTextModelChange(model, status);

      return true;
   }

   return false;
}

bool TextViewController :: insertLine(TextViewModelBase* model, text_t s, size_t length)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->eraseSelection(status);
      docView->insertLine(status, s, length);

      notifyTextModelChange(model, status);

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

      notifyTextModelChange(model, status);

      return true;
   }

   return false;
}

void TextViewController :: setOverwriteMode(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->setOverwriteMode(status, !docView->isOverwriteMode());

      notifyTextModelChange(model, status);
   }
}

void TextViewController :: deleteText(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      if (docView->hasSelection()) {
         docView->eraseSelection(status);
      }
      else docView->eraseLine(status);
   }

   notifyTextModelChange(model, status);
}

void TextViewController :: trim(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->trim(status);
   }

   notifyTextModelChange(model, status);
}

void TextViewController :: eraseLine(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->eraseLine(status);
   }

   notifyTextModelChange(model, status);
}

void TextViewController :: duplicateLine(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->duplicateLine(status);
   }

   notifyTextModelChange(model, status);
}

void TextViewController :: insertBlockText(TextViewModelBase* model, const_text_t s, size_t length)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->blockInserting(status, s, length);
   }

   notifyTextModelChange(model, status);
}

void TextViewController :: deleteBlockText(TextViewModelBase* model, const_text_t s, size_t length)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->blockDeleting(status, s, length);
   }

   notifyTextModelChange(model, status);
}

void TextViewController :: lowerCase(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->toLowercase(status);
   }

   notifyTextModelChange(model, status);
}

void TextViewController :: upperCase(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();
   if (!docView->isReadOnly()) {
      docView->toUppercase(status);
   }

   notifyTextModelChange(model, status);
}

void TextViewController :: moveCaretDown(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveFrameDown(status, model->scrollOffset);
   }
   else docView->moveDown(status, kbShift);

   notifyTextModelChange(model, status);
}

void TextViewController :: moveCaretLeft(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveLeftToken(status, kbShift);
   }
   else docView->moveLeft(status, kbShift);

   notifyTextModelChange(model, status);
}

void TextViewController :: selectWord(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->moveLeftToken(status, false);
   docView->moveRightToken(status, true, true);

   notifyTextModelChange(model, status);
}

void TextViewController::selectAll(TextViewModelBase* model)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->moveFirst(status, false);
   docView->moveEnd(status, true);

   notifyTextModelChange(model, status);
}

void TextViewController :: notifyOnClipboardOperation(ClipboardBase* clipboard)
{

}

bool findBracket(Text* text, TextBookmark& bookmark, text_c starting, text_c ending, bool forward)
{
   // define the upper / lower border of bracket search
   int frameY = 0;
   if (forward)
      frameY = text->getRowCount();

   int counter = 0;
   while (true) {
      text_c ch = text->getChar(bookmark);
      if (ch == starting)
         counter++;
      else if (ch == ending) {
         counter--;
         if (counter == 0)
            return true;
      }

      if (forward) {
         if (!bookmark.moveOn(1) || (bookmark.row() > frameY))
            break;
      }
      else {
         if (!bookmark.moveOn(-1) || bookmark.row() < frameY)
            break;
      }
   }
   return false;
}


void TextViewController :: highlightBrackets(TextViewModelBase* model, DocumentChangeStatus& changeStatus)
{
   auto docView = model->DocView();

   Text* text = docView->getText();
   TextBookmark caret = docView->getCurrentTextBookmark();

   if (docView->clearHighlights())
      changeStatus.formatterChanged = true;

   text_c current_ch = text->getChar(caret);
   if (current_ch == '"')
      return;

   size_t pos = text_str(OPENING_BRACKETS).find(current_ch);
   if (pos != NOTFOUND_POS) {
      docView->addHighlight(caret.position(), STYLE_HIGHLIGHTED_BRACKET);

      if (findBracket(text, caret, OPENING_BRACKETS[pos], CLOSING_BRACKETS[pos], true)) {
         docView->addHighlight(caret.position(), STYLE_HIGHLIGHTED_BRACKET);
      }

      changeStatus.formatterChanged = true;
   }

   pos = text_str(CLOSING_BRACKETS).find(current_ch);
   if (pos != NOTFOUND_POS) {
      pos_t closeBracketPos = caret.position();
      if (findBracket(text, caret, CLOSING_BRACKETS[pos], OPENING_BRACKETS[pos], false)) {
         docView->addHighlight(closeBracketPos, STYLE_HIGHLIGHTED_BRACKET);
         docView->addHighlight(caret.position(), STYLE_HIGHLIGHTED_BRACKET);

         changeStatus.formatterChanged = true;
      }
   }
}

void TextViewController :: notifyTextModelChange(TextViewModelBase* model, DocumentChangeStatus& changeStatus)
{
   model->refresh(changeStatus);

   if (changeStatus.caretChanged && model->highlightBrackets) {
      highlightBrackets(model, changeStatus);
   }

   if (!_notifier)
      return;

   TextViewModelEvent event = { STATUS_DOC_READY, changeStatus };
   _notifier->notify(&event);
}

void TextViewController :: moveCaretRight(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveRightToken(status, kbShift);
   }
   else docView->moveRight(status, kbShift);

   notifyTextModelChange(model, status);
}

void TextViewController :: moveCaretUp(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (!kbCtrl) {
      docView->moveUp(status, kbShift);
   }
   else docView->moveFrameUp(status, model->scrollOffset);

   notifyTextModelChange(model, status);
}

void TextViewController :: moveCaretHome(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveFirst(status, kbShift);
   }
   else docView->moveHome(status, kbShift);

   notifyTextModelChange(model, status);
}

void TextViewController :: moveCaretEnd(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveEnd(status, kbShift);
   }
   else docView->moveLast(status, kbShift);

   notifyTextModelChange(model, status);
}

void TextViewController :: movePageDown(TextViewModelBase* model, bool kbShift)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->movePageDown(status, kbShift);

   notifyTextModelChange(model, status);
}

void TextViewController :: movePageUp(TextViewModelBase* model, bool kbShift)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->movePageUp(status, kbShift);

   notifyTextModelChange(model, status);
}

void TextViewController :: moveToFrame(TextViewModelBase* model, int col, int row, bool kbShift)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   docView->moveToFrame(status, col, row, kbShift);

   notifyTextModelChange(model, status);
}

void TextViewController :: resizeModel(TextViewModelBase* model, Point size)
{
   DocumentChangeStatus status = {};
   auto docView = model->DocView();

   if(docView->getSize() != size) {
      model->resize(size);

      status.frameChanged = true;
   }

   //notifyTextModelChange(model, status);
}

bool TextViewController :: findText(TextViewModelBase* model, FindModel* findModel)
{
   DocumentChangeStatus docStatus = {};

   auto docView = model->DocView();
   if (docView && docView->findLine(docStatus, findModel->text.str(), findModel->matchCase,
      findModel->wholeWord))
   {
      notifyTextModelChange(model, docStatus);

      return true;
   }

   return false;
}

bool TextViewController :: replaceText(TextViewModelBase* model, FindModel* findModel)
{
   DocumentChangeStatus docStatus = {};

   auto docView = model->DocView();
   if (docView && !docView->isReadOnly()) {
      docView->insertLine(docStatus, findModel->newText.str(), findModel->newText.length());
      notifyTextModelChange(model, docStatus);

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
      notifyTextModelChange(model, docStatus);
   }
}
