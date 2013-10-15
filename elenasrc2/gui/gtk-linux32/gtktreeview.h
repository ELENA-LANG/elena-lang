//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TreeView Header File
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef gtktreeviewH
#define gtktreeviewH

#include "gtkcommon.h"

namespace _GUI_
{

// --- TreeViewItem ---

typedef /*HTREEITEM*/void* TreeViewItem;

// --- TreeView ---

class TreeView //: public Control
{
public:
   // !!temporal
   bool isVisible() { return false; }

//   bool isExpanded(TreeViewItem parent);
//
//   TreeViewItem getCurrent();
//   TreeViewItem getChild(TreeViewItem parent);
//   TreeViewItem getNext(TreeViewItem item);
//
//   int getParam(TreeViewItem item);
//   void getCaption(TreeViewItem item, TCHAR* caption, int length);
//
//   void setParam(TreeViewItem item, int param);
//   void setCaption(TreeViewItem item, const TCHAR* caption);
//
//   void select(TreeViewItem item);
//   void expand(TreeViewItem item);
//   void collapse(TreeViewItem item);
//
//   TreeViewItem insertTo(TreeViewItem parent, const TCHAR* caption, int param);
//   void clear(TreeViewItem item);
//   void erase(TreeViewItem item);
//
//   TreeView(Control* owner);
};

} // _GUI_

#endif // gtktreeviewH
