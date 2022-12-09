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
   // --- TreeViewItem ---
   typedef HTREEITEM TreeViewItem;

   // --- TreeView ---
   class TreeView : public ControlBase
   {
      NotifierBase*  _notifier;
      int            _notificationId;

      bool           _persistentSelection;
      bool           _enableIcons;
      int            _iconId;

      HIMAGELIST     _hImages;

   public:
      virtual HWND createControl(HINSTANCE instance, ControlBase* owner);

      void onSelChanged() override;

      void select(TreeViewItem item);
      void expand(TreeViewItem item);
      void collapse(TreeViewItem item);

      TreeViewItem getCurrent();
      TreeViewItem getChild(TreeViewItem parent);
      TreeViewItem getNext(TreeViewItem item);

      size_t getParam(TreeViewItem item);

      size_t readCaption(TreeViewItem item, wchar_t* caption, size_t length);
      void setCaption(TreeViewItem item, wchar_t* caption, size_t length);

      TreeViewItem insertTo(TreeViewItem parent, const wchar_t* caption, size_t param, bool isNode);

      void clear(TreeViewItem item);
      void remove(TreeViewItem item);

      TreeView(int width, int height, NotifierBase* notifier, int notificationId, 
         bool persistentSelection, bool enableIcons = false, int iconId = 0);
      virtual ~TreeView();
   };
}

#endif