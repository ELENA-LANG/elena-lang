//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI StatusBar Implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "winstatusbar.h"

using namespace _GUI_;

// --- StatusBar ---

StatusBar :: StatusBar(Window* owner, int partCount, int* widths)
   : Control(0, 0, 800, 20) // !! temporal
{
   _count = partCount;
   _widths = widths;

   HINSTANCE instance = ((Window*)owner)->_getInstance();

   _handle = ::CreateWindowEx(
      0, STATUSCLASSNAME, _T("Statusbar"), WS_CHILD | SBARS_SIZEGRIP,
      _left, _top, _width, _height, owner->getHandle(), NULL, instance, (LPVOID)this);
}

void StatusBar :: _resize()
{
   Control::_resize();
   
   int* parts = new int[_count];

   int width = _width - 15;
   for (int i = _count - 1 ; i >= 0 ; i--) {
      parts[i] = width;
      width -= _widths[i];
   }
   ::SendMessage(_handle, SB_SETPARTS, (WPARAM)_count, (LPARAM)parts);
}

bool StatusBar :: setText(int index, const wchar_t* str)
{
   if (index > _count) 
      return false;
        
   return (::SendMessage(_handle, SB_SETTEXT, index, (LPARAM)str) == TRUE);
}
