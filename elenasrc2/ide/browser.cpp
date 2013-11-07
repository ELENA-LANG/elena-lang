//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Debugger watch window implementation
//                                              (C)2005-2012, by Alexei Rakov
//---------------------------------------------------------------------------

//#include "idecommon.h"
// --------------------------------------------------------------------------
#include "browser.h"
#include "settings.h"

using namespace _GUI_;
using namespace _ELENA_;

#define CAPTION_LEN  512

typedef String<_text_t, CAPTION_LEN> CaptionString;

#ifdef _WIN32
// --- DebuggerWatch ---

void DebuggerWatch :: expand()
{
   _browser->expand(_root);
}

void DebuggerWatch :: clear()
{
   _browser->clear(_root);
}

//inline void writeAddress(_ELENA_::String& node, size_t address)
//{
//   node.append(_T(" @"));
//   node.appendHex(address);
//}

TreeViewItem DebuggerWatch :: addNode(const _text_t* variableName, const _text_t* className, size_t address)
{
   if (className[0]=='<') {
      CaptionString node(variableName, _T(" = "), className);

      // writeAddress(node, address);

      return _browser->insertTo(_root, node, address);
   }
   else {
      CaptionString node(variableName, _T(" = {"), className, _T("}"));

      //writeAddress(node, address);

      return _browser->insertTo(_root, node, address);
   }
}

void DebuggerWatch :: editNode(TreeViewItem node, const _text_t* variableName, const _text_t* className, size_t address)
{
   if (className[0]=='<') {
      CaptionString name(variableName, _T(" = "), className);

      // writeAddress(name, address);

      _browser->setCaption(node, name);
   }
   else {
      CaptionString name(variableName, _T(" = {"), className, _T("}"));

      // writeAddress(name, address);

      _browser->setCaption(node, name);
   }
   _browser->setParam(node, address);
   _browser->clear(node);
}

void DebuggerWatch :: writeSubWatch(DebugController* controller, TreeViewItem node, size_t address)
{
   if (_deepLevel < 3) {
      DebuggerWatch watch(_browser, node, address, _deepLevel + 1);

      watch.refresh(controller);
   }
   else if (_browser->isExpanded(node)) {
      DebuggerWatch watch(_browser, node, address, 2);

      watch.refresh(controller);
   }
   else refreshNode(node);
}

void DebuggerWatch :: write(DebugController* controller, size_t address, const wchar16_t* variableName, const wchar16_t* className)
{
   wchar_t itemName[CAPTION_LEN + 1];
   size_t nameLen = getlength(variableName);

   TreeViewItem item = _browser->getChild(_root);
   while (item != NULL) {
      size_t itemAddress = _browser->getParam(item);
      _browser->getCaption(item, itemName, CAPTION_LEN);

      if ((getlength(itemName) > nameLen + 1) &&  StringHelper::compare(itemName, variableName, nameLen)
         && itemName[nameLen] == ' ')
      {
         //if (itemAddress != address) {
            editNode(item, variableName, className, address);
         //}
         writeSubWatch(controller, item, address);
         return;
      }
      item = _browser->getNext(item);
   }
   item = addNode(variableName, className, address);

   writeSubWatch(controller, item, address);
}

#ifdef _UNICODE

void DebuggerWatch :: write(DebugController* controller, const char* value)
{
   _browser->clear(_root);

   CaptionString caption(value);

   _browser->insertTo(_root, caption, 0);
}

void DebuggerWatch :: write(DebugController* controller, const wchar16_t* value)
{
   _browser->clear(_root);

   bool     renamed = false;
   _text_t  caption[CAPTION_LEN + 1];
   _browser->getCaption(_root, caption, CAPTION_LEN);

   // cut the value from the caption if any
   if (StringHelper::find(caption, _T(" = {"), -1) == -1) {
      wchar16_t* type = caption + StringHelper::find(caption, '{');

      StringHelper::copy(caption + StringHelper::find(caption, '=') + 2, type, getlength(type) + 1);
      renamed = true;
   }

   // if line too long put the value as a subnode
   if (getlength(caption) + getlength(value) > CAPTION_LEN) {
      if (renamed)
         _browser->setCaption(_root, caption);

      _browser->insertTo(_root, value, 0);
   }
   // else insert the value into caption
   else {
      StringHelper::insert(caption, StringHelper::find(caption, '{'), value);
      _browser->setCaption(_root, caption);
   }
}

#else

void DebuggerWatch :: write(DebugController* controller, const wchar_t* value)
{
   // !! temporal

}

void DebuggerWatch :: write(DebugController* controller, const char* value)
{
   _browser->clear(_root);
   _browser->insertTo(_root, value, 0);
}

#endif

void DebuggerWatch :: write(_ELENA_::DebugController* controller, size_t address, const wchar16_t* variableName, int value)
{
   String<_text_t, 20> number;
   number.append('<');
   if (Settings::hexNumberMode) {
      number.appendHex(value);
      number.append('h');
   }
   else number.appendInt(value);
   number.append('>');

   write(controller, address, variableName, number);
}

void DebuggerWatch :: write(_ELENA_::DebugController* controller, size_t address, const wchar16_t* variableName, double value)
{
   String<_text_t, 20> number;
   number.append('<');
   number.appendDouble(value);
   number.append('>');

   write(controller, address, variableName, number);
}

void DebuggerWatch :: write(_ELENA_::DebugController* controller, size_t address, const wchar16_t* variableName, long long value)
{
   String<_text_t, 20> number;
   number.append('<');
   if (Settings::hexNumberMode) {
      number.appendHex64(value);
      number.append('h');
   }
   else number.appendInt64(value);
   number.append('>');

   write(controller, address, variableName, number);
}

void DebuggerWatch :: write(DebugController* controller, int value)
{
   String<_text_t, 20> number;
   if (Settings::hexNumberMode) {
      number.appendHex(value);
      number.append('h');
   }
   else number.appendInt(value);

   write(controller, number);
}

void DebuggerWatch :: write(DebugController* controller, long long value)
{
   String<_text_t, 20> number;
   if (Settings::hexNumberMode) {
      number.appendHex64(value);
      number.append('h');
   }
   else number.appendInt64(value);

   write(controller, number);
}

void DebuggerWatch :: write(DebugController* controller, double value)
{
   String<_text_t, 20> number;
   number.appendDouble(value);

   write(controller, number);
}

void DebuggerWatch :: refresh(_ELENA_::DebugController* controller)
{
   controller->readContext(this, _objectAddress);
}

// --- DebuggerAutoWatch ---

TreeViewItem DebuggerAutoWatch :: addNode(const _text_t* variableName, const _text_t* className, size_t address)
{
   TreeViewItem item = DebuggerWatch::addNode(variableName, className, address);

   _items.add((int)item, false);

   return item;
}

void DebuggerAutoWatch :: editNode(TreeViewItem node, const _text_t* variableName, const _text_t* className, size_t address)
{
   DebuggerWatch::editNode(node, variableName, className, address);

   refreshNode(node);
}

void DebuggerAutoWatch :: writeSubWatch(DebugController* controller, TreeViewItem node, size_t address)
{
   DebuggerWatch :: writeSubWatch(controller, node, address);

   refreshNode(node);
}

void DebuggerAutoWatch :: refreshNode(TreeViewItem node)
{
   Map<int, bool>::Iterator it = _items.getIt((int)node);
   if (!it.Eof()) {
      *it = false;
   }
   else _items.add((int)node, false);
}

void DebuggerAutoWatch :: clear()
{
   DebuggerWatch :: clear();

   _items.clear();
}

void DebuggerAutoWatch :: refresh(_ELENA_::DebugController* controller)
{
   // mark all auto items
   Map<int, bool>::Iterator it = _items.start();
   while (!it.Eof()) {
      (*it) = true;

      it++;
   }
   controller->readAutoContext(this);

   // remove all marked items
   it = _items.start();
   while (!it.Eof()) {
      if (*it == true) {
         TreeViewItem item = (TreeViewItem)it.key();
         it++;

         _browser->erase(item);
         _items.erase((int)item);
      }
      else it++;
   }
}
#endif

// --- ContextBrowswer ---

ContextBrowser :: ContextBrowser(Control* owner)
#ifdef _WIN32
   : TreeView(owner)
#endif
{
#ifdef _WIN32
   _autoWatch = NULL;

   TreeViewItem autoItem = insertTo(NULL, _T("[auto]"), 0);
   _autoWatch = new DebuggerAutoWatch(this, autoItem);
#endif
}

void ContextBrowser :: browse(DebugController* controller)
{
#ifdef _WIN32
   browse(controller, getCurrent());
   expand(getCurrent());
#endif
}

void ContextBrowser :: browse(DebugController* controller, TreeViewItem current)
{
#ifdef _WIN32
   if (current) {
      size_t address = getParam(current);
      DebuggerWatch subWatch(this, current, address, 0);

      subWatch.refresh(controller);
   }
#endif
}

void ContextBrowser :: refresh(DebugController* controller)
{
#ifdef _WIN32
   if (_autoWatch) {
      _autoWatch->refresh(controller);

      _autoWatch->expand();
   }
#endif
}
