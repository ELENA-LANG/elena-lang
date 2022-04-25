//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Statusbar Header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINSTATUSBAR_H
#define WINSTATUSBAR_H

#include "wincommon.h"

namespace elena_lang
{
   // --- StatusBar ---
   class StatusBar : public ControlBase
   {
      int  _counter;
      int* _widths;

   public:
      void setRectangle(Rectangle rec) override;

      HWND createControl(HINSTANCE instance, ControlBase* owner);

      bool setText(int index, const wchar_t* str);

      StatusBar(int counter, int* widths);
   };
}

#endif // WINSTATUSBAR_H