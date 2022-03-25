//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "windows/winide.h"
#include <windows/Resource.h>

using namespace elena_lang;

bool IDEWindow :: onCommand(int command)
{
   switch (command) {
      case IDM_DEBUG_RUN:
         _controller->projectController.doDebugAction(_model->projectModel, DebugAction::Run);
         break;
      default:
         return false;
   }

   return true;
}

