//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GUI Controller header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef CONTOLLER_H
#define CONTOLLER_H

#include "guieditor.h"

namespace elena_lang
{
   // --- TextViewController ---
   class TextViewController : public TextViewControllerBase
   {
   public:
      void moveCaretDown(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretLeft(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretRight(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretUp(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
   };

}

#endif