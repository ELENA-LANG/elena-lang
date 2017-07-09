//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//      Debugger watch window implementation
//                                              (C)2005-2017, by Alexei Rakov
//---------------------------------------------------------------------------

#include "idecommon.h"
// --------------------------------------------------------------------------
#include "browser.h"
//#include "settings.h"

using namespace _GUI_;
using namespace _ELENA_;

// --- DebuggerWatch ---

void DebuggerWatch :: expand()
{
   _browser->expand(_root);
}

void DebuggerWatch :: clear()
{
   _browser->clear(_root);
}

void* DebuggerWatch :: addNode(_ELENA_::ident_t variableName, _ELENA_::ident_t className, size_t address)
{
   if (className[0]=='<') {
      IdentifierString node(variableName, " = ", className);

      return _browser->newNode(_root, node, address);
   }
   else {
      IdentifierString node(variableName, " = {", className, "}");

      return _browser->newNode(_root, node, address);
   }
}

void DebuggerWatch :: editNode(void* node, _ELENA_::ident_t variableName, _ELENA_::ident_t className, size_t address)
{
   if (className[0]=='<') {
      IdentifierString name(variableName, " = ", className);

      _browser->setCaption(node, name);
   }
   else {
      IdentifierString name(variableName, " = {", className, "}");

      _browser->setCaption(node, name);
   }
   _browser->setParam(node, address);
   _browser->clear(node);
}

void DebuggerWatch :: writeSubWatch(_DebugController* controller, void* node, size_t address)
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

void DebuggerWatch :: append(_DebugController* controller, ident_t variableName, size_t address, size_t vmtAddress)
{
   void* item = _browser->findNodeStartingWith(_root, variableName);

   DebuggerWatch watch(_browser, item, address, 2);
   watch.clear();

   controller->readContext(&watch, address, vmtAddress);
}

void DebuggerWatch :: writeSubWatch(_ELENA_::_DebugController* controller, void* node, size_t address, char* byteArray, int length)
{
   if (_deepLevel < 3 || _browser->isExpanded(node)) {
      DebuggerWatch watch(_browser, node, address, _deepLevel + 1);

      for(int i = 0 ; i < length ; i++) {
         watch.write(controller, i, byteArray[i]);
      }
   }
   else refreshNode(node);
}

void DebuggerWatch :: writeSubWatch(_ELENA_::_DebugController* controller, void* node, size_t address, short* shortArray, int length)
{
   if (_deepLevel < 3 || _browser->isExpanded(node)) {
      DebuggerWatch watch(_browser, node, address, _deepLevel + 1);

      for(int i = 0 ; i < length ; i++) {
         watch.write(controller, i, shortArray[i]);
      }
   }
   else refreshNode(node);
}

void DebuggerWatch :: writeSubWatch(_ELENA_::_DebugController* controller, void* node, size_t address, int* intArray, int length)
{
   if (_deepLevel < 3 || _browser->isExpanded(node)) {
      DebuggerWatch watch(_browser, node, address, _deepLevel + 1);

      for (int i = 0; i < length; i++) {
         watch.write(controller, i, intArray[i]);
      }
   }
   else refreshNode(node);
}

void DebuggerWatch :: write(_DebugController* controller, size_t address, ident_t variableName, ident_t className)
{
   void* item = _browser->findNodeStartingWith(_root, variableName);
   if (item != NULL) {
      //if (itemAddress != address) {
      editNode(item, variableName, className, address);
      //}
   }
   else item = addNode(variableName, className, address);

   writeSubWatch(controller, item, address);
}

void DebuggerWatch :: write(_DebugController* controller, size_t address, _ELENA_::ident_t variableName, char* bytearray, int length)
{
   if (emptystr(variableName)) {
      _browser->clear(_root);

      writeSubWatch(controller, _root, address, bytearray, length);
   }
   else {
      void* item = _browser->findNodeStartingWith(_root, variableName);
      if (item) {
         editNode(item, variableName, "<bytearray>", address);

         writeSubWatch(controller, item, address, bytearray, length);
      }
      else {
         item = addNode(variableName, "<bytearray>", address);

         writeSubWatch(controller, item, address, bytearray, length);
      }
   }
}

void DebuggerWatch :: write(_DebugController* controller, size_t address, _ELENA_::ident_t variableName, short* shortarray, int length)
{
   if (emptystr(variableName)) {
      _browser->clear(_root);

      writeSubWatch(controller, _root, address, shortarray, length);
   }
   else {
      void* item = _browser->findNodeStartingWith(_root, variableName);
      if (item) {
         editNode(item, variableName, "<shortarray>", address);

         writeSubWatch(controller, item, address, shortarray, length);
      }
      else {
         item = addNode(variableName, "<shortarray>", address);

         writeSubWatch(controller, item, address, shortarray, length);
      }
   }
}

void DebuggerWatch :: write(_DebugController* controller, size_t address, _ELENA_::ident_t variableName, int* intarray, int length)
{
   if (emptystr(variableName)) {
      _browser->clear(_root);

      writeSubWatch(controller, _root, address, intarray, length);
   }
   else {
      void* item = _browser->findNodeStartingWith(_root, variableName);
      if (item) {
         editNode(item, variableName, "<intarray>", address);

         writeSubWatch(controller, item, address, intarray, length);
      }
      else {
         item = addNode(variableName, "<intarray>", address);

         writeSubWatch(controller, item, address, intarray, length);
      }
   }
}

void DebuggerWatch :: write(_DebugController* controller, const wide_c* value)
{
   _browser->clear(_root);

   bool renamed = false;
   String<char, CAPTION_LEN + 1> caption;
   _browser->getCaption(_root, caption, CAPTION_LEN);

   // cut the value from the caption if any
   ident_t s(caption);
   if (s.find(" = {", -1) == -1) {
      ident_t type = s + s.find('{');

      size_t len = CAPTION_LEN;
      type.copyTo(caption + s.find('=') + 2, len);
      caption[len] = 0;
      renamed = true;
   }

   // if line too long put the value as a subnode
   if (getlength(caption) + getlength(value) > CAPTION_LEN) {
      if (renamed)
         _browser->setCaption(_root, s);

      _browser->newNode(_root, IdentifierString(value), 0);
   }
   // else insert the value into caption
   else {
      caption.insert(IdentifierString(value), s.find('{'));
      _browser->setCaption(_root, s);
   }
}

void DebuggerWatch::write(_DebugController* controller, const char* value)
{
   _browser->clear(_root);

   bool     renamed = false;
   String<char, CAPTION_LEN + 1> caption;
   _browser->getCaption(_root, caption, CAPTION_LEN);

   // cut the value from the caption if any
   ident_t s = caption.c_str();
   if (s.find(" = {", -1) == -1) {
      ident_t type = s + s.find('{');

      size_t len = CAPTION_LEN;
      type.copyTo(caption + s.find('=') + 2, len);
      caption[len] = 0;
      renamed = true;
   }

   // if line too long put the value as a subnode
   if (getlength(caption) + getlength(value) > CAPTION_LEN) {
      if (renamed)
         _browser->setCaption(_root, s);

      _browser->newNode(_root, value, 0);
   }
   // else insert the value into caption
   else {
      caption.insert(value, s.find('{'));
      _browser->setCaption(_root, s);
   }
}

void DebuggerWatch :: write(_ELENA_::_DebugController* controller, size_t address, _ELENA_::ident_t variableName, int value)
{
   String<char, 20> number;
   number.append('<');
   if (_browser->isHexNumberMode()) {
      number.appendHex(value);
      number.append('h');
   }
   else number.appendInt(value);
   number.append('>');

   write(controller, address, variableName, (ident_t)number);
}

void DebuggerWatch :: write(_ELENA_::_DebugController* controller, size_t address, _ELENA_::ident_t variableName, double value)
{
   String<char, 20> number;
   number.append('<');
   number.appendDouble(value);
   number.append('>');

   write(controller, address, variableName, (ident_t)number);
}

void DebuggerWatch :: write(_ELENA_::_DebugController* controller, size_t address, _ELENA_::ident_t variableName, long long value)
{
   String<char, 20> number;
   number.append('<');
   if (_browser->isHexNumberMode()) {
      number.appendHex64(value);
      number.append('h');
   }
   else number.appendInt64(value);
   number.append('>');

   write(controller, address, variableName, (ident_t)number);
}

void DebuggerWatch :: write(_DebugController* controller, int value)
{
   String<char, 20> number;
   if (_browser->isHexNumberMode()) {
      number.appendHex(value);
      number.append('h');
   }
   else number.appendInt(value);

   write(controller, number);
}

void DebuggerWatch :: write(_DebugController* controller, long long value)
{
   String<char, 20> number;
   if (_browser->isHexNumberMode()) {
      number.appendHex64(value);
      number.append('h');
   }
   else number.appendInt64(value);

   write(controller, number);
}

void DebuggerWatch :: write(_DebugController* controller, double value)
{
   String<char, 20> number;
   number.appendDouble(value);

   write(controller, number);
}

void DebuggerWatch :: write(_DebugController* controller, int index, int value)
{
   IdentifierString node;

   node.append('[');
   node.appendInt(index);
   node.append("] = ");
   node.appendHex(value);

   _browser->newNode(_root, node, 0);
}

void DebuggerWatch :: refresh(_ELENA_::_DebugController* controller)
{
   controller->readContext(this, _objectAddress);
}

// --- DebuggerAutoWatch ---

void* DebuggerAutoWatch :: addNode(_ELENA_::ident_t variableName, _ELENA_::ident_t className, size_t address)
{
   void* item = DebuggerWatch::addNode(variableName, className, address);

   _items.add((size_t)item, false);

   return item;
}

void DebuggerAutoWatch :: editNode(void* node, _ELENA_::ident_t variableName, _ELENA_::ident_t className, size_t address)
{
   DebuggerWatch::editNode(node, variableName, className, address);

   refreshNode(node);
}

void DebuggerAutoWatch :: writeSubWatch(_DebugController* controller, void* node, size_t address)
{
   DebuggerWatch :: writeSubWatch(controller, node, address);

   refreshNode(node);
}

void DebuggerAutoWatch :: refreshNode(void* node)
{
   Map<size_t, bool>::Iterator it = _items.getIt((size_t)node);
   if (!it.Eof()) {
      *it = false;
   }
   else _items.add((size_t)node, false);
}

void DebuggerAutoWatch :: clear()
{
   DebuggerWatch :: clear();

   _items.clear();
}

void DebuggerAutoWatch :: refresh(_ELENA_::_DebugController* controller)
{
   // mark all auto items
   Map<size_t, bool>::Iterator it = _items.start();
   while (!it.Eof()) {
      (*it) = true;

      it++;
   }
   controller->readAutoContext(this);

   // remove all marked items
   it = _items.start();
   while (!it.Eof()) {
      if (*it == true) {
         void* item = (void*)it.key();
         it++;

         _browser->erase(item);
         _items.erase((size_t)item);
      }
      else it++;
   }
}

// --- ContextBrowswer ---

DebugWatch::DebugWatch(_Browser* browser)
{
   void* autoItem = browser->newNode(NULL, "[auto]", 0);
   _autoWatch = new DebuggerAutoWatch(browser, autoItem);
}

void DebugWatch :: refresh(_DebugController* controller)
{
   if (_autoWatch) {
      _autoWatch->refresh(controller);

      _autoWatch->expand();
   }
}
