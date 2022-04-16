//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE View class body File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "view.h"

using namespace elena_lang;

// --- TextViewModel ---

TextViewModel :: TextViewModel(int fontSize)
   : TextViewModelBase(fontSize)
{
   changed = true;
   docView = nullptr;
}

TextViewModel :: ~TextViewModel()
{
}
