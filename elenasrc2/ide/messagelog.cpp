//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      MessageLog class implementation
//                                              (C)2005-2011, by Alexei Rakov
//---------------------------------------------------------------------------

#include "messagelog.h"

using namespace _GUI_;
using namespace _ELENA_;

// --- MessageLog ---

MessageLog :: MessageLog(Control* owner)
   :
#ifdef _WIN32
   ListView(owner),
#endif
   _bookmarks(NULL, freeobj)
{
#ifdef _WIN32
   addLAColumn(_T("Description"), 0, 600);
   addLAColumn(_T("File"), 1, 100);
   addLAColumn(_T("Line"), 2, 100);
   addLAColumn(_T("Column"), 3, 100);
#endif
}

void MessageLog :: addMessage(const tchar_t* message, const tchar_t* file, const tchar_t* row, const tchar_t* col)
{
#ifdef _WIN32
   MessageBookmark* bookmark = new MessageBookmark(file, col, row);

   int index = addItem(message);
   setItemText(file, index, 1);
   setItemText(row, index, 2);
   setItemText(col, index, 3);

   _bookmarks.add(index, bookmark);
#endif
}

void MessageLog :: clear()
{
#ifdef _WIN32
   ListView :: clear();

   _bookmarks.clear();
#endif
}

