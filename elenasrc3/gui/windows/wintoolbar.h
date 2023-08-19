//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Toolbar Header File
//                                             (C)2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINTOOLBAR_H
#define WINTOOLBAR_H

#include "wincommon.h"

namespace elena_lang
{

// --- ToolBarButton ---
struct ToolBarButton
{
   int command;
   int iconId;
};

class ToolBar : public ControlBase
{
   int            _iconSize;

public:
   HWND createControl(HINSTANCE instance, ControlBase* owner,
      ToolBarButton* buttons, size_t counter);

   ToolBar(int iconSize);
};

}

#endif