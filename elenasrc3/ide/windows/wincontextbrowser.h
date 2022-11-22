//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Debug Context Browser Header File
//                                             (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINCONTEXTBROWSER_H
#define WINCONTEXTBROWSER_H

#include "idecommon.h"
#include "windows/wintreeview.h"

namespace elena_lang
{
   class ContextBrowser : public TreeView, public ContextBrowserBase
   {
   public:
      ContextBrowser(int width, int height, NotifierBase* notifier);
   };
}

#endif 