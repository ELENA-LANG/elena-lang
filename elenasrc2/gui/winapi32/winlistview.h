//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI ListView Header File
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef winlistviewH
#define winlistviewH

#include "wincommon.h"

namespace _GUI_
{

// --- ListView ---

class ListView : public Control
{
public:
   //virtual void _resize();

   void _addColumn(const wchar_t* header, int column, int width, int alignment);
   void addLAColumn(const wchar_t* header, int column, int width)
   {
      _addColumn(header, column, width, LVCFMT_LEFT);
   }

   int addItem(const wchar_t* item);
   void setItemText(const wchar_t* item, int row, int column);

   virtual void clear();

   ListView(Control* owner);
};

} // _GUI_

#endif // winlistviewH
