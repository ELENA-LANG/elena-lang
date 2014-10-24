//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Debugger watch window header
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef browserH
#define browserH

#include "gui.h"
//#include "layout.h"
#include "debugcontroller.h"
//#include "menu.h"

namespace _GUI_
{

// --- DebuggerWatch --
class ContextBrowser;

#ifdef _WIN32
class DebuggerWatch : public _ELENA_::_DebuggerWatch
{
protected:
   ContextBrowser* _browser;
   TreeViewItem    _root;
   size_t          _objectAddress;
   size_t          _deepLevel;

   virtual TreeViewItem addNode(const tchar_t* variableName, const tchar_t* className, size_t address);
   virtual void editNode(TreeViewItem node, const tchar_t* variableName, const tchar_t* className, size_t address);

   virtual void refreshNode(TreeViewItem) {}

   virtual void writeSubWatch(_ELENA_::DebugController* controller, TreeViewItem node, size_t address);
   virtual void writeSubWatch(_ELENA_::DebugController* controller, TreeViewItem node, size_t address, char* byteArray, int length);
   virtual void writeSubWatch(_ELENA_::DebugController* controller, TreeViewItem node, size_t address, short* shortArray, int length);

public:
   virtual void expand();
   virtual void clear();

   virtual void write(_ELENA_::DebugController* controller, size_t address,
                        const wchar16_t* variableName, const wchar16_t* className);
   virtual void write(_ELENA_::DebugController* controller, size_t address, 
                        const wchar16_t* variableName, int value);
   virtual void write(_ELENA_::DebugController* controller, size_t address, 
                        const wchar16_t* variableName, double value);
   virtual void write(_ELENA_::DebugController* controller, size_t address, 
                        const wchar16_t* variableName, long long value);
   virtual void write(_ELENA_::DebugController* controller, size_t address, 
                        const wchar16_t* variableName, char* bytearray, int length);
   virtual void write(_ELENA_::DebugController* controller, size_t address, 
                        const wchar16_t* variableName, short* shortarray, int length);
   virtual void write(_ELENA_::DebugController* controller, const char* value);
   virtual void write(_ELENA_::DebugController* controller, const wchar16_t* value);
   virtual void write(_ELENA_::DebugController* controller, int value);
   virtual void write(_ELENA_::DebugController* controller, double value);
   virtual void write(_ELENA_::DebugController* controller, long long value);
   virtual void write(_ELENA_::DebugController* controller, int index, int value);

   virtual void refresh(_ELENA_::DebugController* controller);

   DebuggerWatch(ContextBrowser* browser, TreeViewItem root, size_t objectAddress, size_t deepLevel)
   {
      this->_browser = browser;
      this->_root = root;
      this->_objectAddress = objectAddress;
      this->_deepLevel = deepLevel;
   }
};

// --- DebuggerAutoWatch ---

class DebuggerAutoWatch : public DebuggerWatch
{
   _ELENA_::Map<int, bool> _items;

   virtual TreeViewItem addNode(const tchar_t* variableName, const tchar_t* className, size_t address);
   virtual void editNode(TreeViewItem node, const tchar_t* variableName, const tchar_t* className, size_t address);
   virtual void refreshNode(TreeViewItem);

   virtual void writeSubWatch(_ELENA_::DebugController* controller, TreeViewItem node, size_t address);

public:
//   void showContextMenu(short x, short y);

   virtual void refresh(_ELENA_::DebugController* controller);

   virtual void clear();

   DebuggerAutoWatch(ContextBrowser* browser, TreeViewItem root)
      : DebuggerWatch(browser, root, 0, 0)
   {
   }
};
#endif

// --- ContextBrowser ---

class ContextBrowser : public TreeView
{
   #ifdef _WIN32
   DebuggerAutoWatch* _autoWatch;
   #endif

public:
   void refresh()
   {
      TreeView::refresh();
   }
   void refresh(_ELENA_::DebugController* controller);
   void browse(_ELENA_::DebugController* controller);
   void browse(_ELENA_::DebugController* controller, TreeViewItem current);

   void reset()
   {
   #ifdef _WIN32
      _autoWatch->clear();
   #endif
   }

   ContextBrowser(Control* owner);
   #ifdef _WIN32
   virtual ~ContextBrowser() { freeobj(_autoWatch); }
   #endif
};

} // _GUI_

#endif // browserH
