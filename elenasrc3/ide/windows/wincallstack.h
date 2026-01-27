//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Debug Context Browser Header File
//                                             (C)2022-2026, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINCALLSTACK_H
#define WINCALLSTACK_H

#include "idecommon.h"
#include "windows/winlistview.h"

namespace elena_lang
{
   class CallStackLog : public ListView, public CallstackBase
   {
   //public:
   //   typedef void(*BrowseEventInvoker)(NotifierBase*, size_t, size_t);

   //private:
   //   TreeViewItem         _rootItem;
   //   BrowseEventInvoker   _browseInvoker;

   //   void* findWatchNodeStartingWith(WatchContext* root, ustr_t name) override;

   //   void* addWatchNode(void* parentItem, ustr_t name, ustr_t className, addr_t address) override;
   //   void editWatchNode(void* item, ustr_t name, ustr_t className, addr_t address) override;

   //   void clearNode(void* item) override;
   //   void populateNode(void* item, ustr_t value) override;

   public:
   //   HWND createControl(HINSTANCE instance, ControlBase* owner) override;

   //   void onItemExpand(TreeViewItem item) override;

   //   void expandRootNode() override;
   //   void clearRootNode() override;

   //   void expandNode(size_t param) override;

   //   void refreshCurrentNode() override;

   //   void removeUnused(WatchItems& refreshedItems) override;

      void write(ustr_t moduleName, ustr_t className, ustr_t methodName, ustr_t path, int col, int row, addr_t address) override;
      void write(size_t address) override;

      CallStackLog(/*ContextBrowserModel* model, */int width, int height/*, NotifierBase* notifier, BrowseEventInvoker browseInvoker*/);
   };
}

#endif 