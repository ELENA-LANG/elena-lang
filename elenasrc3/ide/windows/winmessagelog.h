//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Message Log Header File
//                                             (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINMESSAGELOG_H
#define WINMESSAGELOG_H

#include "windows/winlistview.h"

namespace elena_lang
{
   // --- MessageLog ---
   class MessageLog : public ListView
   {
   public:
      HWND createControl(HINSTANCE instance, ControlBase* owner) override;

      MessageLog();
   };
}

#endif // WINMESSAGELOG_H