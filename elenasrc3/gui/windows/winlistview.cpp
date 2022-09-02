//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TabBar Implementation File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "idecommon.h"
#include "winlistview.h"

using namespace elena_lang;

// --- ListView ---

ListView :: ListView(int width, int height)
   : ControlBase(nullptr, 0, 0, width, height)
{

}

HWND ListView :: createControl(HINSTANCE instance, ControlBase* owner)
{
   _handle = ::CreateWindowEx(
      0, WC_LISTVIEW, _title,
      WS_BORDER | WS_CHILD | LVS_REPORT/* | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES*/,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, owner->handle(), nullptr, instance, (LPVOID)this);

   ListView_SetExtendedListViewStyle(_handle, LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

   return _handle;
}

void ListView :: addColumn(const wchar_t* header, int column, int width, int alignment)
{
   LVCOLUMN lvColumn;

   lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
   lvColumn.iSubItem = column;
   lvColumn.pszText = (wchar_t*)header;
   lvColumn.cx = width;
   lvColumn.fmt = alignment;

   ListView_InsertColumn(_handle, column, &lvColumn);
}

void ListView :: addColumn(const wchar_t* header, int column, int width)
{
   addColumn(header, column, width, LVCFMT_LEFT);
}
