//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI ListView Header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINLISTVIEW_H
#define WINLISTVIEW_H

#include "wincommon.h"

namespace elena_lang
{
   // --- ListView ---
   class ListView : public ControlBase
   {
   public:
      virtual HWND createControl(HINSTANCE instance, ControlBase* owner);

      void addColumn(const wchar_t* header, int column, int width, int alignment);
      void addColumn(const wchar_t* header, int column, int width);

      int addRow(const wchar_t* text);
      void setColumnText(const wchar_t* item, int row, int column);

      void onDoubleClick(NMHDR* hdr) override;

      virtual void onItemDblClick(int index) {}

      ListView(int width, int height);
   };
}

#endif