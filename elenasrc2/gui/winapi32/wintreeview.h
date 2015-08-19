//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TreeView Header File
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef wintreeviewH
#define wintreeviewH

#include "wincommon.h"

namespace _GUI_
{

// --- TreeViewItem ---

typedef HTREEITEM TreeViewItem;

// --- TreeView ---

class TreeView : public Control
{
public:
   bool isExpanded(TreeViewItem parent);

   TreeViewItem getCurrent();
   TreeViewItem getChild(TreeViewItem parent);
   TreeViewItem getNext(TreeViewItem item);

   int getParam(TreeViewItem item);
   void getCaption(TreeViewItem item, wchar_t* caption, int length);

   void setParam(TreeViewItem item, int param);
   void setCaption(TreeViewItem item, const wchar_t* caption);

   void select(TreeViewItem item);
   void expand(TreeViewItem item);
   void collapse(TreeViewItem item);

   TreeViewItem insertTo(TreeViewItem parent, const wchar_t* caption, int param);
   void clear(TreeViewItem item);
   void erase(TreeViewItem item);

   TreeView(Control* owner, bool persistentSelection);
};

} // _GUI_

#endif // wintreeviewH
