//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     IDE common classes body File
//                                              (C)2021, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "idecommon.h"
#include "ideview.h"

using namespace elena_lang;

// --- ContextBrowserBase ---

void* ContextBrowserBase :: addOrUpdate(WatchContext* context, ustr_t variableName, ustr_t className)
{
   void* item = findWatchNodeStartingWith(context, variableName);
   if (item != nullptr) {
      editWatchNode(item, variableName, className, context->address);
   }
   else item = addWatchNode(context->root, variableName, className, context->address);

   return item;
}

void ContextBrowserBase::addOrUpdateDWORD(WatchContext* context, ustr_t variableName, int value)
{
   void* item = findWatchNodeStartingWith(context, variableName);
   if (item != nullptr) {
      editWatchNode(item, variableName, "<int>", context->address);
   }
   else item = addWatchNode(context->root, variableName, "<int>", context->address);

   WatchContext dwordContext = { item };

   populateDWORD(&dwordContext, value);
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

void ContextBrowserBase :: addOrUpdateQWORD(WatchContext* context, ustr_t variableName, long long value)
{
   void* item = findWatchNodeStartingWith(context, variableName);
   if (item != nullptr) {
      editWatchNode(item, variableName, "<long>", context->address);
   }
   else item = addWatchNode(context->root, variableName, "<long>", context->address);

   WatchContext dwordContext = { item };

   populateQWORD(&dwordContext, value);
}

void ContextBrowserBase :: populateQWORD(WatchContext* context, unsigned long long value)
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
