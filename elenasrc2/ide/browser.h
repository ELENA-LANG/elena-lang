//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Debugger watch window header
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#ifndef browserH
#define browserH

#include "gui.h"
#include "debugging.h"

namespace _GUI_
{

#define CAPTION_LEN  512

// --- _Browser ---

class _Browser
{
public: 
   virtual bool isHexNumberMode() = 0;

   virtual bool isExpanded(void* node) = 0;
   virtual void expand(void* node) = 0;
   virtual void clear(void* node) = 0;
   virtual void erase(void* node) = 0;

   virtual void* newNode(void* parent, _ELENA_::ident_t name, size_t param) = 0;

   virtual void* findNodeStartingWith(void* node, _ELENA_::ident_t name)   = 0;

   virtual void getCaption(void* node, char* caption, size_t length) = 0;
   virtual void setCaption(void* node, _ELENA_::ident_t caption) = 0;

   virtual void setParam(void* node, size_t param) = 0;
};

// --- DebuggerWatch --

class DebuggerWatch : public _ELENA_::_DebuggerWatch
{
protected:
   _Browser* _browser;
   void*     _root;
   size_t    _objectAddress;
   size_t    _deepLevel;

   virtual void* addNode(_ELENA_::ident_t variableName, _ELENA_::ident_t className, size_t address);
   virtual void editNode(void* node, _ELENA_::ident_t variableName, _ELENA_::ident_t className, size_t address);

   virtual void refreshNode(void*) {}

   virtual void writeSubWatch(_ELENA_::_DebugController* controller, void* node, size_t address);
   virtual void writeSubWatch(_ELENA_::_DebugController* controller, void* node, size_t address, char* byteArray, int length);
   virtual void writeSubWatch(_ELENA_::_DebugController* controller, void* node, size_t address, short* shortArray, int length);
   virtual void writeSubWatch(_ELENA_::_DebugController* controller, void* node, size_t address, int* intArray, int length);

public:
   virtual void expand();
   virtual void clear();

   virtual void write(_ELENA_::_DebugController* controller, size_t address,
                        _ELENA_::ident_t variableName, _ELENA_::ident_t className);
   virtual void write(_ELENA_::_DebugController* controller, size_t address, 
                        _ELENA_::ident_t variableName, int value);
   virtual void write(_ELENA_::_DebugController* controller, size_t address, 
                        _ELENA_::ident_t variableName, double value);
   virtual void write(_ELENA_::_DebugController* controller, size_t address, 
                        _ELENA_::ident_t variableName, long long value);
   virtual void write(_ELENA_::_DebugController* controller, size_t address, 
                        _ELENA_::ident_t variableName, char* bytearray, int length);
   virtual void write(_ELENA_::_DebugController* controller, size_t address, 
                        _ELENA_::ident_t variableName, short* shortarray, int length);
   virtual void write(_ELENA_::_DebugController* controller, size_t address, 
                        _ELENA_::ident_t variableName, int* intarray, int length);
   virtual void write(_ELENA_::_DebugController* controller, const char* value);
   virtual void write(_ELENA_::_DebugController* controller, const _ELENA_::wide_c* value);
   virtual void write(_ELENA_::_DebugController* controller, int value);
   virtual void write(_ELENA_::_DebugController* controller, double value);
   virtual void write(_ELENA_::_DebugController* controller, long long value);
   virtual void write(_ELENA_::_DebugController* controller, int index, int value);

   virtual void append(_ELENA_::_DebugController* controller, _ELENA_::ident_t variableName, size_t address, size_t vmtAddress);

   virtual void refresh(_ELENA_::_DebugController* controller);

   DebuggerWatch(_Browser* browser, void* root, size_t objectAddress, size_t deepLevel)
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
   _ELENA_::Map<size_t, bool> _items;

   virtual void* addNode(_ELENA_::ident_t variableName, _ELENA_::ident_t className, size_t address);
   virtual void editNode(void* node, _ELENA_::ident_t variableName, _ELENA_::ident_t className, size_t address);
   virtual void refreshNode(void*);

   virtual void writeSubWatch(_ELENA_::_DebugController* controller, void* node, size_t address);

public:
   virtual void refresh(_ELENA_::_DebugController* controller);

   virtual void clear();

   DebuggerAutoWatch(_Browser* browser, void* root)
      : DebuggerWatch(browser, root, 0, 0)
   {
   }
};

// --- DebugWatch ---

class DebugWatch
{
   DebuggerAutoWatch* _autoWatch;

public:
   void refresh(_ELENA_::_DebugController* controller);

   void reset()
   {
      _autoWatch->clear();
   }

   DebugWatch(_Browser* browser);
   virtual ~DebugWatch() { _ELENA_::freeobj(_autoWatch); }
};

} // _GUI_

#endif // browserH
