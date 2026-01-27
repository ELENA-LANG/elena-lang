//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Message Log Header File
//                                             (C)2022-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINMESSAGELOG_H
#define WINMESSAGELOG_H

#include "idecommon.h"
#include "windows/winlistview.h"

namespace elena_lang
{
   // --- MessageLog ---
   class MessageLog : public ListView, public ErrorLogBase, public MessageLogBase
   {
   public:
      HWND createControl(HINSTANCE instance, ControlBase* owner) override;

      void addMessage(text_str message, text_str file, text_str row, text_str col) override;

      MessageLogInfo getMessage(int index) override;

      void clearMessages() override;

      void onItemDblClick(int index) override;

      MessageLog(NotifierBase* notifier, SelectionEventInvoker invoker);
   };
}

#endif // WINMESSAGELOG_H