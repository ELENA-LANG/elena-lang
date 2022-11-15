//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Message Log Implementation File
//                                             (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include <tchar.h>

#include "winmessagelog.h"

using namespace elena_lang;

// --- MessageLog ---

MessageLog :: MessageLog(NotifierBase* notifier, int highlightCode)
   : ListView(50, 50), _list({}), _paths(nullptr)
{
   _notifier = notifier;
   _highlightCode = highlightCode;
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

   // NOTE : the path should be cloned
   PathString filePath(file);

   path_t pathStr = (*filePath).clone();

   _list.add(index, { pathStr, StrConvertor::toInt(row, 10), StrConvertor::toInt(col, 10) });
}

MessageLogInfo MessageLog :: getMessage(int index)
{
   return _list.get(index);
}

void MessageLog :: clearMessages()
{
   _paths.clear();
   _list.clear();

   clearRows();
}

void MessageLog :: onItemDblClick(int index)
{
   _notifier->notifyMessage(_highlightCode, index);
}
