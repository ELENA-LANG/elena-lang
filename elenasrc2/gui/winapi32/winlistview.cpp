//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI ListView Implementation
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#include "winlistview.h"

using namespace _GUI_;

// --- ListView ---

ListView :: ListView(Control* owner)
   : Control(0, 0, 800, 20) // !! temporal
{
   HINSTANCE instance = ((Control*)owner)->_getInstance();

   _handle = ::CreateWindowEx(
      0, WC_LISTVIEW, NULL, WS_BORDER | WS_CHILD | LVS_REPORT/* | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES*/,
      _left, _top, _width, _height, owner->getHandle(), NULL, instance, (LPVOID)this);

   ListView_SetExtendedListViewStyle(_handle, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
}

void ListView :: _addColumn(const wchar_t* header, int column, int width, int alignment)
{
   LVCOLUMN lvColumn; 
   
   lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM; 
   lvColumn.iSubItem = column;
   lvColumn.pszText = (wchar_t*)header;	
   lvColumn.cx = width;    
   lvColumn.fmt = alignment;
	  
   ListView_InsertColumn(_handle, column, &lvColumn);
}

int ListView :: addItem(const wchar_t* item)
{
   if (_ELENA_::emptystr(item))  
      return -1;
     
   int row = ListView_GetItemCount(_handle);

   LVITEM lvItem; 
   
   lvItem.mask = LVIF_TEXT; 
   lvItem.state = 0;    
   lvItem.stateMask = 0; 
   lvItem.iItem = row;
   lvItem.iSubItem = 0;
   lvItem.pszText = (wchar_t*)item; 

   row = (int)::SendMessage(_handle, LVM_INSERTITEM, 0, (LPARAM)&lvItem);

   return row;
}

void ListView :: setItemText(const wchar_t* item, int row, int column)
{
   if (_ELENA_::emptystr(item))  
      return;

   LVITEM lvItem; 
   
   lvItem.mask = LVIF_TEXT; 
   lvItem.state = 0;    
   lvItem.stateMask = 0; 
   lvItem.iItem = row;
   lvItem.iSubItem = column;
   lvItem.pszText = (wchar_t*)item; 

   ::SendMessage(_handle, LVM_SETITEMTEXT, row, (LPARAM)&lvItem);
}

void ListView :: clear()
{
   ListView_DeleteAllItems(_handle);
}
