//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Win32 EditFrame container File
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINTEXTFRAME_H
#define WINTEXTFRAME_H

#include "wintabbar.h"
#include "editframe.h"

namespace elena_lang
{
   // --- TextViewFrame --
   class TextViewFrame : public MultiTabControl, TextViewListener
   {
   protected:
      TextViewModel*          _model;

   public:
      void onDocumentNew(int index) override;
      void onDocumentSelect(int index) override;
      //void afterDocumentSelect(int index) override;
      //void onDocumentRename(int index) override;
      void onDocumentModeChanged(int index, bool modifiedMode) override;
      void beforeDocumentClose(int index) override;
      void onDocumentClose(int index) override;

      void onSelChanged() override;

      TextViewFrame(NotifierBase* notifier, bool withAbovescore, ControlBase* view, TextViewModel* model, SelectionEventInvoker invoker);
   };

}

#endif

