//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TreeView Header File
//                                             (C)2021-2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINTREEVIEW_H
#define WINTREEVIEW_H

#include "wincommon.h"

namespace elena_lang
{
   // --- TreeView ---
   class TreeView : public ControlBase
   {
      bool _persistentSelection;
      bool _enableIcons;
      int  _iconId;

      HIMAGELIST _hImages;

   public:
      virtual HWND createControl(HINSTANCE instance, ControlBase* owner);

      TreeView(int width, int height, bool persistentSelection, bool enableIcons = false, int iconId = 0);
      virtual ~TreeView();
   };
}

#endif