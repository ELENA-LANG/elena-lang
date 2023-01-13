//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Status Bar Header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINIDESTATUSBAR_H
#define WINIDESTATUSBAR_H

#include "windows/winstatusbar.h"
#include "ideview.h"

namespace elena_lang
{
   // --- IDEStatusBar ---

   class IDEStatusBar : public StatusBar, DocumentNotifier
   {
      IDEModel* _model;
      IDEStatus _status;
      bool      _pendingIDESettings;

      void onDocumentUpdate(DocumentChangeStatus& changeStatus) override;

      void setIDEStatus(IDEStatus status);

   public:
      void setRectangle(Rectangle rec) override;

      void refresh() override;

      IDEStatusBar(IDEModel* model);
   };
}

#endif