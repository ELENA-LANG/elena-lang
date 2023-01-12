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

   class IDEStatusBar : public StatusBar, IDEListener
   {
      IDEModel* _model;
      bool      _pendingIDESettings;

      void onIDEChange();

   public:
      void setRectangle(Rectangle rec) override;

      IDEStatusBar(IDEModel* model);
   };
}

#endif