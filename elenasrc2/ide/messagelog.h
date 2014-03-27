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

// --- MessageBookmark ---

struct MessageBookmark
{
   tchar_t* file;
   size_t   col, row;

   MessageBookmark(const tchar_t* file, const tchar_t* col, const tchar_t* row)
   {
      this->file = _ELENA_::StringHelper::clone(file);
      this->col = _ELENA_::StringHelper::strToInt(col);
      this->row = _ELENA_::StringHelper::strToInt(row);
   }

   ~MessageBookmark()
   {
      _ELENA_::freestr(file);
   }
};

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
