//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINIDE_H
#define WINIDE_H

#include "windows/winsdi.h"
#include "windows/windialogs.h"
#include "idecontroller.h"
#include "ideview.h"

namespace elena_lang
{
   class IDEWindow : public SDIWindow
   {
      Dialog         dialog;

      HINSTANCE      _instance;

      IDEModel*      _model;
      IDEController* _controller;

      int            _textFrameId;

      void onModelChange(ExtNMHDR* hdr);
      void onTabSelChanged(HWND wnd);

      bool onCommand(int command) override;
      void onNotify(NMHDR* hdr) override;
      void onActivate() override;

      void newFile();
      void openFile();
      void saveFile();
      void closeFile();
      void exit();

      void undo();

   public:
      IDEWindow(wstr_t title, IDEController* controller, IDEModel* model, HINSTANCE instance, 
         int textFrameId);
   };

}

#endif