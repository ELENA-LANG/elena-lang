//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      CallStackLog class implementation
//                                              (C)2005-2011, by Alexei Rakov
//---------------------------------------------------------------------------

#include "callstack.h"

using namespace _GUI_;
using namespace _ELENA_;

// --- MessageLog ---

CallStackLog :: CallStackLog(Control* owner)
   :
#ifdef _WIN32
   ListView(owner),
#endif
   _bookmarks(NULL, freeobj)
{
#ifdef _WIN32
   addLAColumn(_T("Method"), 0, 600);
   addLAColumn(_T("File"), 1, 100);
   addLAColumn(_T("Line"), 2, 100);
#endif
}

void CallStackLog :: addCall(const wchar16_t* module, const wchar16_t* method, const tchar_t* path, int col, int row)
{
#ifdef _WIN32
   MessageBookmark* bookmark = new MessageBookmark(module, path, col, row);

   wchar16_t rowStr[10];
   StringHelper::intToStr(row, rowStr, 10);

   int index = addItem(method);
   setItemText(path, index, 1);
   setItemText(rowStr, index, 2);

   _bookmarks.add(index, bookmark);
#endif
}

void CallStackLog :: clear()
{
#ifdef _WIN32
   ListView :: clear();

   _bookmarks.clear();
#endif
}

void CallStackLog :: refresh(DebugController* controller)
{
#ifdef _WIN32
   if (isVisible()) {
      CallStackWatch watch(this);

      clear();
      controller->readCallStack(&watch);
   }
#endif
}

// --- CallStackWatch ---

void CallStackWatch :: write(const wchar16_t* moduleName, const wchar16_t* className, const wchar16_t* methodName, const wchar16_t* path, int col, int row, size_t address)
{
   IdentifierString source(className);
   if (!emptystr(methodName))
   {
      source.append('.');
      source.append(methodName);
   }
   source.append(' ');
   source.append('<');
   source.appendHex(address);
   source.append('>');

   _log->addCall(moduleName, source, path, col, row);
}

void CallStackWatch :: write(size_t address)
{
   IdentifierString source;
   source.append('<');
   source.appendHex(address);
   source.append('>');

   _log->addCall(NULL, source, _T("unknown"), 0, 0);
}

