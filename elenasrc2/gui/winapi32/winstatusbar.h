//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI StatusBar Header File
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winstatusbarH
#define winstatusbarH

#include "wincommon.h"

namespace _GUI_
{

// --- StatusBar ---

class StatusBar : public Control
{
   int  _count;
   int* _widths;

public:
   bool setText(int index, const wchar_t* str);

   StatusBar(Window* owner, int partCount, int* widths);

   virtual void _resize();
};

} // _GUI_

#endif // winstatusbarH
