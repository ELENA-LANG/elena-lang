//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Linux IDE Process Output Header File
//                                             (C)2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef GTKOUTPUT_H
#define GTKOUTPUT_H

#include "gtklinux/gtkcommon.h"

namespace elena_lang
{
   // --- ProcessOutput ---
   class ProcessOutput : public Gtk::ScrolledWindow
   {
   protected:
      Gtk::TextView       _output;

   public:
      Gtk::TextView* getOutput() { return &_output; }

      ProcessOutput();
   };

   // --- CompilerOutput --
   class CompilerOutput : public ProcessOutput
   {
   };
}

#endif // GTKOUTPUT_H
