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
      typedef Map<int, MessageLogInfo> MessageList;
      typedef List<path_t, freepath>   Paths;

      MessageList   _list;
      Paths         _paths;
      NotifierBase* _notifier;
      int           _highlightCode;

   public:
      HWND createControl(HINSTANCE instance, ControlBase* owner) override;

      void addMessage(text_str message, text_str file, text_str row, text_str col) override;

      MessageLogInfo getMessage(int index) override;

      void clearMessages() override;

      void onItemDblClick(int index) override;

      MessageLog(NotifierBase* notifier, int highlightCode);
   };
}

#endif // WINMESSAGELOG_H