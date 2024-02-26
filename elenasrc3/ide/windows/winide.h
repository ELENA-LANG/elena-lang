//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Header File
//                                             (C)2021-2024, by Aleksey Rakov
//---------------------------------------------------------------------------

#ifndef WINIDE_H
#define WINIDE_H

#include "windows/winsdi.h"
#include "windows/windialogs.h"
#include "idecontroller.h"
#include "ideview.h"
#include "windowlist.h"

namespace elena_lang
{
   // --- Notifications ---
   struct ModelNMHDR
   {
      NMHDR nmhrd;
      int   status;
   };

   struct TextViewModelNMHDR : public ModelNMHDR
   {
      DocumentChangeStatus docStatus;
   };

   struct TextFrameSelectionNMHDR : public ModelNMHDR
   {
      int index;
   };

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

   // --- IDENotificationFormatter ---
   class IDENotificationFormatter : public EventFormatterBase
   {
      IDENotificationFormatter() = default;

      void sendTextViewModelEvent(TextViewModelEvent* event, WindowApp* app);
      void sendTextFrameSelectionEvent(SelectionEvent* event, WindowApp* app);
      void sendStartUpEvent(StartUpEvent* event, WindowApp* app);

   public:
      static IDENotificationFormatter& getInstance()
      {
         static IDENotificationFormatter instance; // Guaranteed to be destroyed.
         // Instantiated on first use.
         return instance;

      }  
      
      IDENotificationFormatter(IDENotificationFormatter const&) = delete;
      void operator=(IDENotificationFormatter const&) = delete;

      void sendMessage(EventBase* event, WindowApp* app) override;
   };

   // --- IDEWindow ---
   class IDEWindow : public SDIWindow
   {
      FileDialog        fileDialog;
      FileDialog        projectDialog;
      MessageDialog     messageDialog;
      ProjectSettings   projectSettingsDialog;
      FindDialog        findDialog, replaceDialog;
      GoToLineDialog    gotoDialog;
      WindowListDialog  windowDialog;
      Clipboard         clipboard;
      AboutDialog       aboutDialog;
      EditorSettings    editorSettingsDialog;

      ViewFactoryBase*  _viewFactory;

      HINSTANCE         _instance;
      HWND              _tabTTHandle;

      IDEModel*         _model;
      IDEController*    _controller;

      WindowList        _windowList;
      RecentList        _recentFileList;
      RecentList        _recentProjectList;

      DocumentNotifiers _docViewListener;

      void enableMenuItemById(int id, bool doEnable, bool toolBarItemAvailable);

      void onTextModelChange(TextViewModelNMHDR* rec);
      void onTextFrameSel(TextFrameSelectionNMHDR* rec);
      void onIDEStatusChange(ModelNMHDR* rec);
      void onStartup(ModelNMHDR* rec);
      void onLayoutChange(ModelNMHDR* rec);

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

      bool isTabToolTip(HWND handle);

      void onToolTip(NMTTDISPINFO* toolTip);
      void onTabTip(NMTTDISPINFO* toolTip);

      void onLayoutChange(NotificationStatus status);
      void onIDEChange(NotificationStatus status);

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
      void onProjectViewSel(int index);

      void onColorSchemeChange();

      void onStatusBarChange();

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
      void closeAllButActive();
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
      void trim();
      void eraseLine();
      void duplicateLine();
      void upperCase();
      void lowerCase();

      void search();
      void searchNext();
      void replace();
      void goToLine();

      void includeFile();

      void openHelp();
      void showAbout();

      void refreshDebugNode();

      void onDebuggerStart();
      void onDebuggerHook();
      void onDebuggerUpdate(StatusNMHDR* rec);
      void onDebuggerSourceNotFound();
      void onDocumentUpdate(DocumentChangeStatus& changeStatus);
      void onProgrammRunning(StatusNMHDR* rec);

   public:
      void populate(size_t counter, GUIControlBase** children) override;

      IDEWindow(wstr_t title, IDEController* controller, IDEModel* model, HINSTANCE instance, ViewFactoryBase* viewFactory);
   };

}

#endif