//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINIDE_H
#define WINIDE_H

#include "windows/winsdi.h"
#include "idecontroller.h"
#include "ideview.h"

namespace elena_lang
{
   class IDEWindow : public SDIWindow
   {
      IDEModel*      _model;
      IDEController* _controller;

      bool onCommand(int command) override;

   public:
      IDEWindow(wstr_t title, IDEController* controller, IDEModel* model)
         : SDIWindow(title)
      {
         this->_controller = controller;
         this->_model = model;
      }
   };

}

#endif