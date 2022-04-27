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

void TextViewController :: newDocument(TextViewModelBase* model, ustr_t name)
{
   Text* text = new Text(_eolMode);
   text->create();

   model->addDocumentView(name, text);
}

void TextViewController :: openDocument(TextViewModelBase* model, ustr_t name, path_t path, FileEncoding encoding)
{
   Text* text = new Text(_eolMode);
   text->load(path, encoding, false);

   model->addDocumentView(name, text);
}

void TextViewController :: selectDocument(TextViewModelBase* model, ustr_t name)
{
   model->selectDocumentView(name);
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
