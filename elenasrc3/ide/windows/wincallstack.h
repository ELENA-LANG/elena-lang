//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Debug Context Browser Header File
//                                             (C)2022-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINCALLSTACK_H
#define WINCALLSTACK_H

#include "idecommon.h"
#include "windows/winlistview.h"

namespace elena_lang
{
   class CallStackLog : public ListView, public CallstackBase, public MessageLogBase
   {
   public:
      HWND createControl(HINSTANCE instance, ControlBase* owner) override;

      void onItemDblClick(int index) override;

      void write(ustr_t moduleName, ustr_t className, ustr_t methodName, ustr_t path, int col, int row, addr_t address) override;
      void write(size_t address) override;

      void clear() override;

      MessageLogInfo getMessage(int index) override;

      CallStackLog(int width, int height, NotifierBase* notifier, SelectionEventInvoker invoker);
   };
}

#endif 