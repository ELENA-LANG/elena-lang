//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Debug Context Browser Implementation File
//                                             (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <tchar.h>

#include "wincontextbrowser.h"

using namespace elena_lang;

// --- ContextBrowser --

ContextBrowser :: ContextBrowser(int width, int height, NotifierBase* notifier)
   : TreeView(width, height, notifier, 0, false, false)
{
   
}
