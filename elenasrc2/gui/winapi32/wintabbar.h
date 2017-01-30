//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI TablBar Header File
//                                              (C)2005-2015, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef wintabbarH
#define wintabbarH

#include "wincommon.h"

namespace _GUI_
{

// --- CustomTabBar ---

class CustomTabBar : public Control
{
protected:
   HWND     _owner;

   bool     _withAbovescore;
   bool     _notSelected;

public:
   virtual void _onDrawItem(DRAWITEMSTRUCT* item);

   HWND getOwner() const { return _owner; }

   int getCurrentIndex();
   int getTabCount();

   void* getTabParam(int index);
   void getTabName(int index, wchar_t* name, int length);

   void addTab(int index, const wchar_t* name, void* param);
   void selectTab(int index);
   void deleteTab(int index);
   void renameTab(int index, const wchar_t* newName);

   CustomTabBar(Window* owner, bool withAbovescore);
};

// --- MultiTabView ---

class MultiTabView : public CustomTabBar
{
protected:
   Control* _child;

   virtual void onSetFocus();

public:
   int addTabView(const wchar_t* name, void* param);
   void eraseTabView(int index);
   void renameTabView(int index, const wchar_t* newName);

   virtual void _setWidth(int width);
   virtual void _setHeight(int height);

   virtual void _resize();

   void setChild(Control* child)
   {
      _child = child;
   }

   virtual void setFocus();

   virtual void onTabChange(int)
   {
   }

   MultiTabView(Window* owner, bool withAbovescore, Control* child);
};

// --- TabBar ---

class TabBar : public CustomTabBar
{
   _ELENA_::List<Control*> _children;
   Control*                _child;

public:
   void addTabChild(const wchar_t* name, Control* window);
   void removeTabChild(Control* window);

   void selectTabChild(Control* window);
   void selectTabChild(int index);
   void selectLastTabChild()
   {
      selectTabChild(_children.Count() - 1);
   }

   virtual void _setWidth(int width);
   virtual void _setHeight(int height);

   virtual void _resize();

   virtual void refresh();

   void onTabChange(int)
   {
      refresh();
   }

   TabBar(Window* owner, bool withAbovescore);
};

} // _GUI_

#endif // wintabbarH
