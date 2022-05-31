//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Win32 EditFrame container File
//                                             (C)2021-2022, by Aleksey Rakov
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
      TextViewModel* _model;

   public:
      void onDocumentNew(int index, int notifyMessage) override;
      void onDocumentSelect(int index) override;
      void afterDocumentSelect(int index) override;
      void onDocumentRename(int index) override;

      void onSelChanged() override;

      TextViewFrame(NotifierBase* notifier, bool withAbovescore, ControlBase* view, TextViewModel* model);
   };

}

#endif

