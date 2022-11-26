//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Debug Context Browser Implementation File
//                                             (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#include "wincontextbrowser.h"
// --------------------------------------------------------------------------
#include <tchar.h>
#include "elena.h"

using namespace elena_lang;

constexpr auto CAPTION_LEN = 512;

typedef String<text_t, CAPTION_LEN> CaptrionString;

// --- ContextBrowser --

ContextBrowser :: ContextBrowser(int width, int height, NotifierBase* notifier)
   : TreeView(width, height, notifier, 0, false, false)
{
}

HWND ContextBrowser :: createControl(HINSTANCE instance, ControlBase* owner)
{
   HWND h = TreeView::createControl(instance, owner);

   _rootItem = insertTo(nullptr, _T("[auto]"), 0, true);

   return h;
}

void* ContextBrowser :: findWatchNodeStartingWith(WatchContext* context, ustr_t name)
{
   WideMessage wideName(name);
   size_t nameLen = wideName.length();

   text_c caption[CAPTION_LEN];

   TreeViewItem parent = context->root ? (TreeViewItem)context->root : _rootItem;

   TreeViewItem item = getChild(parent);
   while (item != nullptr) {
      size_t len = readCaption(item, caption, CAPTION_LEN);

      if (nameLen < len && wstr_t(caption).compareSub(*wideName, 0, nameLen) 
         && caption[nameLen] == ' ')
      {
         return item;
      }

      item = getNext(item);
   }

   return nullptr;
}

inline void formatCaption(WideMessage& retVal, ustr_t name, ustr_t className, addr_t address)
{
   retVal.appendUstr(name);
   if (!address) {
      retVal.appendUstr(" = <nil>");
   }
   else if (className.empty()) {
      retVal.appendUstr(" = <unknown>");
   }
   //else if (className[0] == '<') {
   //   retVal.appendUstr(" = ");
   //   retVal.appendUstr(className);
   //}
   else {
      retVal.appendUstr(" = {");
      retVal.appendUstr(className);
      retVal.appendUstr("}");

   }
}

void* ContextBrowser :: addWatchNode(void* parentItem, ustr_t name, ustr_t className, addr_t address)
{
   TreeViewItem parent = parentItem ? (TreeViewItem)parentItem : _rootItem;

   WideMessage caption;
   formatCaption(caption, name, className, address);

   return insertTo(parent, *caption, address, true);
}

void ContextBrowser :: editWatchNode(void* item, ustr_t name, ustr_t className, addr_t address)
{
   WideMessage caption;
   formatCaption(caption, name, className, address);

   setCaption((TreeViewItem)item, (wchar_t*)caption.str(), caption.length());
}

void ContextBrowser :: clearNode(void* item)
{
   clear((TreeViewItem)item);
}

void ContextBrowser :: populateNode(void* item, ustr_t value)
{
   text_c caption[CAPTION_LEN];
   size_t len = readCaption((TreeViewItem)item, caption, CAPTION_LEN);

   // cut the value from the caption if any
   wstr_t s = caption;

   WideMessage wideValue(value);
   if (s.length() + value.length() > CAPTION_LEN) {
      insertTo((TreeViewItem)item, *wideValue, 0, false);
   }
   else {
      pos_t pos = s.find('{');
      if (pos != NOTFOUND_POS)
         StrUtil::insert(caption, pos, value.length(), wideValue.str());

      setCaption((TreeViewItem)item, caption, getlength(caption));
   }

}

void ContextBrowser::expandRootNode()
{
   expand(_rootItem);
}

void ContextBrowser::removeUnused(WatchItems& refreshedItems)
{
   TreeViewItem current = getChild(_rootItem);
   while (current != nullptr) {
      bool found = false;
      for (int i = 0; i < refreshedItems.count_pos(); i++) {
         if (refreshedItems.get(i) == current) {
            found = true;
            break;
         }
      }
      if (!found) {
         TreeViewItem unused = current;
         current = getNext(current);

         remove(unused);
      }
      else current = getNext(current);
   }
}