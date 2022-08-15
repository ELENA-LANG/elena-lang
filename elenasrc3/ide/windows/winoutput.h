//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Process Output Header File
//                                             (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------


#ifndef WINOUTPUT_H
#define WINOUTPUT_H

#include "windows/wincommon.h"

namespace elena_lang
{
   // --- ProcessOutput ---
   class ProcessOutput : public ControlBase
   {
   protected:
      WNDPROC           _editProc;

      virtual LRESULT OutputProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

   public:
      static LRESULT CALLBACK Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

      HWND createControl(HINSTANCE instance, ControlBase* owner);

      ProcessOutput();
   };

   // --- CompilerOutput ---
   class CompilerOutput : public ProcessOutput
   {
   public:
      CompilerOutput();
   };
}

#endif // WINOUTPUT_H