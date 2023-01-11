//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     EditFrame container File
//                                                 (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "ideview.h"

using namespace elena_lang;

// --- IDEModel ---

void IDEModel :: changeStatus(IDEStatus status)
{
   this->status = status;

 //  onIDEChange();
}

void IDEModel :: attachListener(IDEListener* listener)
{
   listeners.add(listener);

   //listener->onIDEChange();
}

//void IDEModel :: onIDEChange()
//{
//   for (auto it = listeners.start(); !it.eof(); ++it) {
//      (*it)->onIDEChange();
//   }
//}