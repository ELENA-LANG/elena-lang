//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI ToolBar Header File
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef wintoolbarH
#define wintoolbarH

#include "wincommon.h"

namespace _GUI_
{

// --- ToolBarButton ---
struct ToolBarButton
{
   int command;
   int iconId; 
};

// --- ToolBar ---

class ToolBar : public Control
{
public:
   void enableItemById(int id, bool doEnable);

   ToolBar(Window* owner, int iconSize, int count, ToolBarButton* buttons);
};

} // _GUI_

#endif // wintoolbarH
