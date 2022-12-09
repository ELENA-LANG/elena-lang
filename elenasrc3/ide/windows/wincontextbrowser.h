//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Debug Context Browser Header File
//                                             (C)2022, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINCONTEXTBROWSER_H
#define WINCONTEXTBROWSER_H

#include "idecommon.h"
#include "windows/wintreeview.h"

namespace elena_lang
{
   class ContextBrowser : public TreeView, public ContextBrowserBase
   {
      TreeViewItem _rootItem;

      void* findWatchNodeStartingWith(WatchContext* root, ustr_t name) override;

      void* addWatchNode(void* parentItem, ustr_t name, ustr_t className, addr_t address) override;
      void editWatchNode(void* item, ustr_t name, ustr_t className, addr_t address) override;

      void clearNode(void* item) override;
      void populateNode(void* item, ustr_t value) override;

   public:
      HWND createControl(HINSTANCE instance, ControlBase* owner) override;

      void expandRootNode() override;

      void removeUnused(WatchItems& refreshedItems) override;

      ContextBrowser(int width, int height, NotifierBase* notifier);
   };
}

#endif 