//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE common classes body File
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "idecommon.h"
#include "ideview.h"

using namespace elena_lang;

// --- ContextBrowserBase ---

void* ContextBrowserBase :: addOrUpdate(WatchContext* context, ustr_t variableName, ustr_t className)
{
   void* item = findWatchNodeStartingWith(context, variableName);
   if (item != nullptr) {
      clearNode(item);

      editWatchNode(item, variableName, className, context->address);
   }
   else item = addWatchNode(context->root, variableName, className, context->address);

   return item;
}

void* ContextBrowserBase :: addOrUpdateDWORD(WatchContext* context, ustr_t variableName, int value)
{
   void* item = findWatchNodeStartingWith(context, variableName);
   if (item != nullptr) {
      editWatchNode(item, variableName, "<int>", context->address);
   }
   else item = addWatchNode(context->root, variableName, "<int>", context->address);

   WatchContext dwordContext = { item };

   populateDWORD(&dwordContext, value);

   return item;
}

void* ContextBrowserBase :: addOrUpdateUINT(WatchContext* context, ustr_t variableName, int value)
{
   void* item = findWatchNodeStartingWith(context, variableName);
   if (item != nullptr) {
      editWatchNode(item, variableName, "<uint>", context->address);
   }
   else item = addWatchNode(context->root, variableName, "<uint>", context->address);

   WatchContext dwordContext = { item };

   populateUINT(&dwordContext, value);

   return item;
}

void* ContextBrowserBase :: addOrUpdateWORD(WatchContext* context, ustr_t variableName, short value)
{
   void* item = findWatchNodeStartingWith(context, variableName);
   if (item != nullptr) {
      editWatchNode(item, variableName, "<short>", context->address);
   }
   else item = addWatchNode(context->root, variableName, "<short>", context->address);

   WatchContext dwordContext = { item };

   populateWORD(&dwordContext, value);

   return item;
}

void* ContextBrowserBase :: addOrUpdateBYTE(WatchContext* context, ustr_t variableName, int value)
{
   void* item = findWatchNodeStartingWith(context, variableName);
   if (item != nullptr) {
      editWatchNode(item, variableName, "<byte>", context->address);
   }
   else item = addWatchNode(context->root, variableName, "<byte>", context->address);

   WatchContext dwordContext = { item };

   populateDWORD(&dwordContext, value);

   return item;
}

void ContextBrowserBase :: populateWORD(WatchContext* context, unsigned short value)
{
   String<char, 20> number;
   /*if (_browser->isHexNumberMode()) {
      number.appendHex(value);
      number.append('h');
   }
   else*/ number.appendInt(value);

   clearNode(context->root);
   populateNode(context->root, number.str());

}

void ContextBrowserBase :: populateDWORD(WatchContext* context, unsigned value)
{
   String<char, 20> number;
   /*if (_browser->isHexNumberMode()) {
      number.appendHex(value);
      number.append('h');
   }
   else*/ number.appendInt(value);

   clearNode(context->root);
   populateNode(context->root, number.str());

}

void ContextBrowserBase :: populateUINT(WatchContext* context, unsigned value)
{
   String<char, 20> number;
   /*if (_browser->isHexNumberMode()) {
      number.appendHex(value);
      number.append('h');
   }
   else*/ number.appendUInt(value);

   clearNode(context->root);
   populateNode(context->root, number.str());

}

void ContextBrowserBase :: populateString(WatchContext* context, const char* value)
{
   clearNode(context->root);
   populateNode(context->root, value);
}

void ContextBrowserBase::populateWideString(WatchContext* context, const wide_c* value)
{
   clearNode(context->root);

   IdentifierString s(value);
   populateNode(context->root, *s);
}

void* ContextBrowserBase :: addOrUpdateQWORD(WatchContext* context, ustr_t variableName, long long value)
{
   void* item = findWatchNodeStartingWith(context, variableName);
   if (item != nullptr) {
      editWatchNode(item, variableName, "<long>", context->address);
   }
   else item = addWatchNode(context->root, variableName, "<long>", context->address);

   WatchContext dwordContext = { item };

   populateQWORD(&dwordContext, value);

   return item;
}

void ContextBrowserBase :: populateQWORD(WatchContext* context, long long value)
{
   String<char, 40> number;
   /*if (_browser->isHexNumberMode()) {
      number.appendHex(value);
      number.append('h');
   }
   else*/ number.appendLong(value);

   clearNode(context->root);
   populateNode(context->root, number.str());
}

void* ContextBrowserBase :: addOrUpdateFLOAT64(WatchContext* context, ustr_t variableName, double value)
{
   void* item = findWatchNodeStartingWith(context, variableName);
   if (item != nullptr) {
      editWatchNode(item, variableName, "<double>", context->address);
   }
   else item = addWatchNode(context->root, variableName, "<double>", context->address);

   WatchContext dwordContext = { item };

   populateFLOAT64(&dwordContext, value);

   return item;
}

void ContextBrowserBase :: populateFLOAT64(WatchContext* context, double value)
{
   String<char, 40> number;
   number.appendDouble(value);

   clearNode(context->root);
   populateNode(context->root, number.str());
}

// --- SelectionEvent ---

SelectionEvent :: SelectionEvent(int id, int index)
   : EventBase(0), _eventId(id), _index(index)
{
}

int SelectionEvent :: eventId()
{
   return _eventId;
}

// --- ParamSelectionEvent ---

ParamSelectionEvent :: ParamSelectionEvent(int id, size_t param)
   : EventBase(0), _eventId(id), _param(param)
{
}

int ParamSelectionEvent :: eventId()
{
   return _eventId;
}

// --- LayoutEvent ---

LayoutEvent::LayoutEvent(int status)
   : EventBase(status)
{
}

int LayoutEvent::eventId()
{
   return EVENT_LAYOUT;
}

// --- StartUpEvent ---

StartUpEvent::StartUpEvent(int status)
   : EventBase(status)
{
}

int StartUpEvent::eventId()
{
   return EVENT_STARTUP;
}
// --- TextViewModelEvent ---

int TextViewModelEvent::eventId()
{
   return EVENT_TEXTVIEW_MODEL_CHANGED;
}

// --- ContextMenuEvent ---

ContextMenuEvent :: ContextMenuEvent(int id, int x, int y, bool hasSelection)
   : EventBase(0), _eventId(id), _x(x), _y(y), _hasSelection(hasSelection)
{
}

int ContextMenuEvent :: eventId()
{
   return _eventId;
}

