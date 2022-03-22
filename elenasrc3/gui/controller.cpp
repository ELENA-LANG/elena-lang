//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI Controller implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "controller.h"

using namespace elena_lang;

// --- TextViewController ---

void TextViewController :: moveCaretDown(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   if (kbCtrl) {
      model->docView->moveFrameDown();
   }
   else model->docView->moveDown(kbShift);

}

void TextViewController :: moveCaretLeft(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   if (kbCtrl) {
      model->docView->moveLeftToken(kbShift);
   }
   else model->docView->moveLeft(kbShift);
}

void TextViewController :: moveCaretRight(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   if (kbCtrl) {
      model->docView->moveRightToken(kbShift);
   }
   else model->docView->moveRight(kbShift);
}

void TextViewController :: moveCaretUp(TextViewModelBase* model, bool kbShift, bool kbCtrl)
{
   if (kbCtrl) {
      model->docView->moveFrameUp();
   }
   else model->docView->moveUp(kbShift);
}
