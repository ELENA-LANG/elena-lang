//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      WinAPI Splitter class header 
//                                              (C)2022, by Aleksex Rakov
//---------------------------------------------------------------------------

#ifndef WINSPLITTER_H
#define WINSPLITTER_H

#include "wincommon.h"

namespace elena_lang
{
   class Splitter : public WindowBase
   {
      HINSTANCE     _instance;

      NotifierBase* _notifier;
      int           _notifyCode;

      ControlBase*  _client;
      bool          _vertical;

      POINT         _srcPos;
      bool          _mouseCaptured;

      void onButtonDown(Point point, bool kbShift);
      void onButtonUp();
      void onMove();

      void shiftOn(int delta);

   public:
      static void registerSplitterWindow(HINSTANCE hInstance, wstr_t className, bool vertical);

      HWND create(HINSTANCE instance, wstr_t className, ControlBase* owner) override;

      bool visible() override;

      Rectangle getRectangle() override;
      void setRectangle(Rectangle rec) override;

      //void onResize() override;

      LRESULT proceed(UINT message, WPARAM wParam, LPARAM lParam) override;

      Splitter(NotifierBase* notifier, int notifyCode, ControlBase* client, bool vertical);
   };
}

#endif

