//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Process Output Header File
//                                             (C)2022-2024, by Aleksey Rakov
//---------------------------------------------------------------------------


#ifndef WINOUTPUT_H
#define WINOUTPUT_H

#include "windows/wincommon.h"
#include "idecommon.h"

namespace elena_lang
{
   // --- ProcessOutput ---
   class ProcessOutput : public ControlBase, public ProcessListenerBase
   {
   protected:
      ProcessBase*      _process;

      WNDPROC           _editProc;
      bool              _readOnly;

      virtual LRESULT OutputProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

      void onOutput(const char* s) override;
      void onErrorOutput(const char* s) override;
      void afterExecution(int exitCode, int extraArg) override {}

   public:
      wchar_t* getValue() override;
      void clearValue() override;

      static LRESULT CALLBACK Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

      HWND createControl(HINSTANCE instance, ControlBase* owner);

      ProcessOutput(ProcessBase* process, bool readOnly);
   };

   // --- CompilerOutput ---
   class CompilerOutput : public ProcessOutput
   {
   public:
      typedef void(*EventInvoker)(NotifierBase*, int, int);

   private:
      NotifierBase* _notifier;
      EventInvoker  _eventInvoker;

      void afterExecution(int exitCode, int extraArg) override;

   public:
      CompilerOutput(NotifierBase* notifier, EventInvoker invoker);
   };

   // --- VMConsoleInteractive ---
   class VMConsoleInteractive : public ProcessOutput
   {
   public:
      VMConsoleInteractive(ProcessBase* process);
   };

}

#endif // WINOUTPUT_H