//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TreeView Header File
//                                             (C)2021-2024, by Aleksey Rakov
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
   public:
      typedef void(*SelectionEventInvoker)(NotifierBase*, size_t);

   protected:
      NotifierBase* _notifier;

   private:
      SelectionEventInvoker   _selectionInvoker;

      bool                    _persistentSelection;
      bool                    _enableIcons;
      int                     _iconId;

      HIMAGELIST              _hImages;

   public:
      virtual HWND createControl(HINSTANCE instance, ControlBase* owner);

      bool setColor(int index, Color color) override;

      void onSelChanged() override;

      virtual void onItemExpand(TreeViewItem item) {}

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

      TreeViewItem hitTest(short x, short y);

      void clear(TreeViewItem item);
      void remove(TreeViewItem item);

      TreeView(int width, int height, NotifierBase* notifier, bool persistentSelection, 
         SelectionEventInvoker invoker, bool enableIcons = false, int iconId = 0);
      virtual ~TreeView();
   };
}

#endif