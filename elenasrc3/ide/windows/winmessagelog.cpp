//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Message Log Implementation File
//                                             (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "winmessagelog.h"

using namespace elena_lang;

// --- MessageLog ---

MessageLog :: MessageLog()
   : ListView(50, 50)
{

}

HWND MessageLog :: createControl(HINSTANCE instance, ControlBase* owner)
{
   auto h = ListView::createControl(instance, owner);

   addColumn(_T("Description"), 0, 600);
   addColumn(_T("File"), 1, 100);
   addColumn(_T("Line"), 2, 100);
   addColumn(_T("Column"), 3, 100);

   return h;
}

void MessageLog :: addMessage(text_str message, text_str file, text_str row, text_str col)
{
   int index = addRow(message);
   setColumnText(file, index, 1);
   setColumnText(row, index, 2);
   setColumnText(col, index, 3);

}