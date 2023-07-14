//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Header File
//                                             (C)2021-2023, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINIDE_H
#define WINIDE_H

#include "windows/winsdi.h"
#include "windows/windialogs.h"
#include "idecontroller.h"
#include "ideview.h"

namespace elena_lang
{
   // --- Clipboard ---
   class Clipboard : public ClipboardBase
   {
      ControlBase* _owner;

      bool begin();
      void clear();
      void copy(HGLOBAL buffer);
      HGLOBAL get();
      void end();

      HGLOBAL createBuffer(size_t size);
      wchar_t* allocateBuffer(HGLOBAL buffer);
      void freeBuffer(HGLOBAL buffer);

   public:
      static bool isAvailable();

      bool copyToClipboard(DocumentView* docView) override;
      void pasteFromClipboard(DocumentChangeStatus& status, DocumentView* docView) override;

      Clipboard(ControlBase* owner);
   };

   // --- IDEWindow ---
   class IDEWindow : public SDIWindow, DocumentNotifier
   {
      FileDialog        fileDialog;
      FileDialog        projectDialog;
      MessageDialog     messageDialog;
      ProjectSettings   projectSettingsDialog;
      FindDialog        findDialog, replaceDialog;
      GoToLineDialog    gotoDialog;
      Clipboard         clipboard;

      HINSTANCE         _instance;

      IDEModel*         _model;
      IDEController*    _controller;

      void onStatusChange(StatusNMHDR* rec);
      void onSelection(SelectionNMHDR* rec);
      void onTreeItem(TreeItemNMHDR* rec);
      void onComplition(CompletionNMHDR* rec);
      void onContextMenu(ContextMenuNMHDR* rec);
      //void onModelChange(ExtNMHDR* hdr);
      //void onNotifyMessage(ExtNMHDR* hdr);

      void onDebugWatch();

      void onDoubleClick(NMHDR* hdr);
      void onRClick(NMHDR* hdr);
      void onDebugWatchRClick(size_t index);

      void onTabSelChanged(HWND wnd);
      void onTreeSelChanged(HWND wnd);
      void onChildRefresh(int controlId);
      void onTreeItemExpanded(NMTREEVIEW* rec);

      void onLayoutChange(NotificationStatus status);
      void onIDEChange(NotificationStatus status);
      void onTextFrameChange(NotificationStatus status);

      bool onCommand(int command) override;
      void onNotify(NMHDR* hdr) override;
      void onActivate() override;
      bool onClose() override;

      void onComilationStart();
      void onCompilationEnd(int exitCode);
      void onErrorHighlight(int index);
      void onDebugResult(int code);
      void onDebugWatchBrowse(size_t item, size_t param);

      void onProjectChange(bool empty);
      void onProjectViewSel(size_t index);

      bool toggleTabBarWindow(int child_id);
      void toggleWindow(int child_id);

      void toggleProjectView(bool open);
      void openResultTab(int controlIndex);
      void closeResultTab(int controlIndex);
      void setChildFocus(int controlIndex);

      void newFile();
      void openFile();
      void saveFile();
      void saveAll();
      void saveProject();
      void closeFile();
      void closeAll();
      void newProject();
      void openProject();
      void closeProject();
      void exit() override;

      void undo();
      void redo();
      bool copyToClipboard();
      void pasteFromClipboard();
      void deleteText();
      void commentText();
      void uncommentText();
      void selectAll();

      void search();
      void searchNext();
      void replace();
      void goToLine();

      void includeFile();

      void openHelp();

      void refreshDebugNode();

      void onIDEViewUpdate(bool forced);
      void onDebuggerStart();
      void onDebuggerHook();
      void onDebuggerUpdate(StatusNMHDR* rec);
      void onDocumentUpdate(DocumentChangeStatus& changeStatus) override;

   public:
      IDEWindow(wstr_t title, IDEController* controller, IDEModel* model, HINSTANCE instance);
   };

}

#endif