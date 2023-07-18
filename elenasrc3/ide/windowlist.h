//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE Window list header File
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINDOWLIST_H
#define WINDOWLIST_H

#include "idecontroller.h"
#include "menuhistory.h"

namespace elena_lang
{
   // --- WindowList ---
   class WindowList : public MenuHistoryBase
   {
      IDEController* _controller;

   public:
      void beforeDocumentClose(int) override;

      void onDocumentModeChanged(int, bool) override {}

      void onDocumentNew(int index) override;
      void onDocumentClose(int index) override {}
      void onDocumentSelect(int index) override;

      bool select(int index);

      WindowList(IDEController* controller, TextViewModelBase* model);
   };

} // elena:lang

#endif // WINDOWLIST_H
