//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Message Log Header File
//                                             (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINMESSAGELOG_H
#define WINMESSAGELOG_H

#include "idecommon.h"
#include "windows/winlistview.h"

namespace elena_lang
{
   // --- MessageLog ---
   class MessageLog : public ListView, public ErrorLogBase
   {
   public:
      HWND createControl(HINSTANCE instance, ControlBase* owner) override;

      void addMessage(text_str message, text_str file, text_str row, text_str col) override;

      MessageLog();
   };
}

#endif // WINMESSAGELOG_H