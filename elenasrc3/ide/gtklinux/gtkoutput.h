//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Linux IDE Process Output Header File
//                                             (C)2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKOUTPUT_H
#define GTKOUTPUT_H

#include "idecommon.h"
#include "gtklinux/gtkcommon.h"

namespace elena_lang
{
   // --- ProcessOutput ---
   class ProcessOutput : public Gtk::ScrolledWindow, public ProcessListenerBase
   {
   protected:
      Gtk::TextView       _output;

   public:
      Gtk::TextView* getOutput() { return &_output; }

      void clear()
      {
         _output.get_buffer()->set_text("");
      }

      void onOutput(const char* s) override;
      void onErrorOutput(const char* s) override;

      ProcessOutput();
   };

   // --- CompilerOutput --
   class CompilerOutput : public ProcessOutput
   {
      BroadcasterBase* _eventBroadcaster;

   public:
      void afterExecution(int exitCode, int extraArg) override;

      CompilerOutput(BroadcasterBase* eventBroadcaster);
   };
}

#endif // GTKOUTPUT_H
