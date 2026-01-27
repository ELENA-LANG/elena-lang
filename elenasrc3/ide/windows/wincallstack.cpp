//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Debug Context Browser Implementation File
//                                             (C)2022-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "wincallstack.h"
// --------------------------------------------------------------------------
//#include <tchar.h>
//#include "elena.h"

using namespace elena_lang;

// --- CallStackLog --

CallStackLog::CallStackLog(/*ContextBrowserModel* model, */int width, int height/*, NotifierBase* notifier,
   BrowseEventInvoker browseInvoker*/)
   : ListView(width, height/*, notifier, false, nullptr, false*/)//, _rootItem(nullptr)
{
//   _browseInvoker = browseInvoker;
//   _model = model;
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

   TextString widePathStr(path);
   setColumnText(*widePathStr, index, 1);

   String<wchar_t, 10> rowStr;
   rowStr.appendInt(row);
   setColumnText(rowStr.str(), index, 2);

   //MessageBookmark* bookmark = new MessageBookmark(moduleName, path.c_str(), col, row);
   //_log->_bookmarks.add(index, bookmark);
}

void CallStackLog :: write(size_t address)
{
   TextString wideMessageStr;
   wideMessageStr.append('<');
   wideMessageStr.appendInt(address);
   wideMessageStr.append('>');

   addRow(*wideMessageStr);
}
