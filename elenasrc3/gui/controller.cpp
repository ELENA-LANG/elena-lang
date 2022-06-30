//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI Controller implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "controller.h"
#include "elena.h"

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

   model->addDocumentView(name, text, nullptr, notifyMessage);
}

bool TextViewController :: openDocument(TextViewModelBase* model, ustr_t name, path_t path, 
   FileEncoding encoding, int notifyMessage)
{
   Text* text = new Text(_settings.eolMode);
   if (!text->load(path, encoding, false))
      return false;

   model->addDocumentView(name, text, path, notifyMessage);

   return true;
}

void TextViewController :: selectDocument(TextViewModelBase* model, ustr_t name)
{
   model->selectDocumentView(name);
}

void TextViewController :: closeDocument(TextViewModelBase* model, ustr_t name, 
   int notifyMessage)
{
   model->closeDocumentView(name, notifyMessage);
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

void TextViewController :: pasteFromClipboard(TextViewModelBase* model, ClipboardBase* clipboard)
{
   auto docView = model->DocView();
   if (!docView->status.readOnly) {
      clipboard->pasteFromClipboard(docView);
   }
}

void TextViewController :: moveCaretDown(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveFrameDown();
   }
   else docView->moveDown(kbShift);
}

void TextViewController :: moveCaretLeft(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveLeftToken(kbShift);
   }
   else docView->moveLeft(kbShift);
}

void TextViewController :: moveCaretRight(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveRightToken(kbShift);
   }
   else docView->moveRight(kbShift);
}

void TextViewController :: moveCaretUp(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   auto docView = model->DocView();

   if (kbCtrl) {
      docView->moveFrameUp();
   }
   else docView->moveUp(kbShift);
}
