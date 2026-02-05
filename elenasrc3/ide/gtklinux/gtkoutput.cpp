//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     Linux IDE Process Output Implementation File
//                                             (C)2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "gtklinux/gtkoutput.h"

using namespace elena_lang;

// --- ProcessOutput ---

ProcessOutput :: ProcessOutput()
{
   set_child(_output);
}

void ProcessOutput :: onOutput(const char* output)
{
   size_t len = getlength(output);

   Gtk::TextIter iter = _output.get_buffer()->end();
   _output.get_buffer()->insert(iter, output, output + len);
}

void ProcessOutput :: onErrorOutput(const char* s)
{
}

// --- CompilerOutput ---

CompilerOutput :: CompilerOutput(BroadcasterBase* eventBroadcaster)
{
   _eventBroadcaster = eventBroadcaster;
}

void CompilerOutput :: afterExecution(int exitCode, int postponedAction)
{
   CompletionEvent event = { EVENT_COMPILATION_END, exitCode, postponedAction };

   _eventBroadcaster->sendMessage(&event);
}
