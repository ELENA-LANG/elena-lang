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
