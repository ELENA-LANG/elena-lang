//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      WinAPI Splitter class header 
//                                             (C)2022-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINSPLITTER_H
#define WINSPLITTER_H

#include "wincommon.h"

namespace elena_lang
{
   class Splitter : public WindowBase
   {
      HINSTANCE            _instance;

      NotifierBase*        _notifier;
      int                  _notifyCode;
      NotificationStatus   _notifyStatus;

      ControlBase*         _client;
      bool                 _vertical;

      POINT                _srcPos;
      bool                 _mouseCaptured;

      int                  _cursor;

      void onButtonDown(Point point, bool kbShift);
      void onButtonUp();
      void onMove();
      bool onSetCursor() override;

      void shiftOn(int delta);

   public:
      static void registerSplitterWindow(HINSTANCE hInstance, wstr_t className, bool vertical);

      HWND create(HINSTANCE instance, wstr_t className, ControlBase* owner) override;

      bool visible() override;

      Rectangle getRectangle() override;
      void setRectangle(Rectangle rec) override;

      void refresh() override;

      LRESULT proceed(UINT message, WPARAM wParam, LPARAM lParam) override;

      Splitter(NotifierBase* notifier, int notifyCode, NotificationStatus notifyStatus , ControlBase* client, bool vertical);
   };
}

#endif

