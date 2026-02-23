//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Debug Context Browser Implementation File
//                                             (C)2022-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "wincallstack.h"
// --------------------------------------------------------------------------
#include <tchar.h>

using namespace elena_lang;

// --- CallStackLog --

CallStackLog::CallStackLog(int width, int height, NotifierBase* notifier, SelectionEventInvoker invoker)
   : ListView(width, height), MessageLogBase(notifier, invoker)
{
}

void CallStackLog :: write(ustr_t moduleName, ustr_t className, ustr_t methodName, ustr_t path, int col, int row, addr_t address)
{
   IdentifierString  messageStr(className);
   PathString        pathStr(path);

   if (!methodName.empty()) {
      messageStr.append(methodName);
   }
   messageStr.append(' ');
   messageStr.append('<');
   messageStr.appendHex(address);
   messageStr.append('>');

   TextString wideMessageStr(*messageStr);
   int index = addRow(*wideMessageStr);

   setColumnText(*pathStr, index, 1);

   String<wchar_t, 10> rowStr;
   rowStr.appendInt(row);
   setColumnText(rowStr.str(), index, 2);

   addLog(index, *pathStr, row - 1, col);
}

void CallStackLog :: write(size_t address)
{
   TextString wideMessageStr;
   wideMessageStr.append('<');
   wideMessageStr.appendInt(address);
   wideMessageStr.append('>');

   addRow(*wideMessageStr);
}

void CallStackLog :: clear()
{
   clearRows();
   clearLog();
}

void CallStackLog :: onItemDblClick(int index)
{
   if (index >= 0)
      _invoker(_notifier, index);
}

HWND CallStackLog :: createControl(HINSTANCE instance, ControlBase* owner)
{
   auto h = ListView::createControl(instance, owner);

   addColumn(_T("Method"), 0, 600);
   addColumn(_T("File"), 1, 100);
   addColumn(_T("Line"), 2, 100);

   return h;
}

MessageLogInfo CallStackLog :: getMessage(int index)
{
   return _list.get(index);
}
