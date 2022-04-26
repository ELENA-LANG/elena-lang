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
   protected:
      EOLMode  _eolMode;

   public:
      void openDocument(TextViewModelBase* model, ustr_t name, path_t path, FileEncoding encoding) override;
      void selectDocument(TextViewModelBase* model, ustr_t name) override;

      //void newDocument(TextViewModelBase* model) override;

      void moveCaretDown(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretLeft(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretRight(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;
      void moveCaretUp(TextViewModelBase* model, bool kbShift, bool kbCtrl) override;

      TextViewController(EOLMode eolMode)
         : _eolMode(eolMode)
      {
      }
   };

}

#endif