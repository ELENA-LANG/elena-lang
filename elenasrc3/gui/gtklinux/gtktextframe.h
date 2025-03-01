//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Linux EditFrame container File
//                                             (C)2021-2025, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKTEXTFRAME_H
#define GTKTEXTFRAME_H

#include "gtktabbar.h"
#include "editframe.h"
#include "gtktextview.h"

namespace elena_lang
{
   // --- TextViewFrame ---
   class TextViewFrame : public TabBar, TextViewListener
   {
   protected:
      TextViewModel*          _model;
      ViewStyles*             _styles;
      TextViewControllerBase* _controller;

   public:
      //void on_text_model_change(TextViewModelEvent event);

      void onDocumentNew(int index) override;
      void onDocumentSelect(int index) override;
      void onDocumentModeChanged(int index, bool modifiedMode) override;
      void beforeDocumentClose(int index) override;
      void onDocumentClose(int index, bool empty) override;

      TextViewFrame(TextViewModel* model, TextViewControllerBase* controller, ViewStyles* styles);
   };
}

#endif
