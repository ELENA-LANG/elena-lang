//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI Statusbar Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "winstatusbar.h"

using namespace elena_lang;

// --- StatusBar ---

StatusBar :: StatusBar(int counter, int* widths)
   : ControlBase(nullptr, 0, 0, 800, 20), _counter(counter), _widths(widths)
{
}

void StatusBar :: setRectangle(Rectangle rec)
{
   ControlBase::setRectangle(rec);

   int* parts = new int[_counter];

   int width = rec.width() - 15;
   for (int i = _counter - 1; i >= 0; i--) {
      parts[i] = width;
      width -= _widths[i];
   }
   ::SendMessage(_handle, SB_SETPARTS, (WPARAM)_counter, (LPARAM)parts);
}

bool StatusBar :: setText(int index, const wchar_t* str)
{
   if (_counter < index)
      return false;

   return (::SendMessage(_handle, SB_SETTEXT, index, (LPARAM)str) == TRUE);
}

HWND StatusBar :: createControl(HINSTANCE instance, ControlBase* owner)
{
   _handle = ::CreateWindowEx(
      0, STATUSCLASSNAME, _title,
      WS_CHILD | SBARS_SIZEGRIP,
      0, 0, 0, 0x16, owner->handle(), nullptr, instance, (LPVOID)this);

   return _handle;
}
