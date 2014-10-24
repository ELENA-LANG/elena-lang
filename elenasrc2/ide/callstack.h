//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      CallStackLog class header
//                                              (C)2005-2014, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef callstackH
#define callstackH

#include "ide.h"
#include "debugcontroller.h"

namespace _GUI_
{

// --- CallStackLog ---

class CallStackLog : public ListView
{
   _ELENA_::Map<int, MessageBookmark*> _bookmarks;

public:
   MessageBookmark* getBookmark(int index) { return _bookmarks.get(index); }

   void addCall(const wchar16_t* module, const wchar16_t* method, const tchar_t* path, int col, int row);

   virtual void clear();

   void refresh(_ELENA_::DebugController* controller);

   CallStackLog(Control* owner);
};

// --- DebuggerCallStack ---

class CallStackWatch : public _ELENA_::_DebuggerCallStack
{
   CallStackLog* _log;

public:
   virtual void write(const wchar16_t* moduleName, const wchar16_t* className, const wchar16_t* methodName, const wchar16_t* path, 
                        int col, int row, size_t address);
   virtual void write(size_t address);

   CallStackWatch(CallStackLog* log)
   {
      _log = log;
   }
};

} // _GUI_

#endif // callstackH
