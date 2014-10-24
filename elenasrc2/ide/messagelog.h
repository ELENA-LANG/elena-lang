//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      MessageLog class header
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef messagelogH
#define messagelogH

#include "ide.h"

namespace _GUI_
{

// --- MessageLog ---

class MessageLog : public ListView
{
   _ELENA_::Map<int, MessageBookmark*> _bookmarks;

public:

   MessageBookmark* getBookmark(int index) { return _bookmarks.get(index); }

   void addMessage(const tchar_t* message, const tchar_t* file, const tchar_t* row, const tchar_t* col);

   virtual void clear();

   MessageLog(Control* owner);
};

} // _GUI_

#endif // messagelogH
