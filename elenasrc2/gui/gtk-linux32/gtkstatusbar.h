//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     GTK+ StatusBar Header File
//                                              (C)2005-2010, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef gtkstatusbarH
#define gtkstatusbarH

#include "gtkcommon.h"

namespace _GUI_
{

// --- StatusBar ---

class StatusBar //: public Control
{
//   GtkWidget** _items;

public:
   bool setText(int index, const char* str);

//   StatusBar(Window* owner, int partCount, int* widths);
//   virtual ~StatusBar();
};

} // _GUI_

#endif // gtkstatusbarH
