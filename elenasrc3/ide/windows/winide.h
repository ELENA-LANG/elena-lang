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
   // --- ContextMenuNMHDR ---
   struct ContextMenuNMHDR
   {
      NMHDR nmhrd;
      int   x, y;
      bool  hasSelection;
   };

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

   struct SelectionNMHDR : public ModelNMHDR
   {
      int index;
   };

   struct CompletionNMHDR : public ModelNMHDR
   {
      int exitCode;
      int postponedAction;
   };

   struct BrowseNMHDR : public ModelNMHDR
   {
      size_t item;
      size_t param;
   };

   struct ParamSelectionNMHDR : public ModelNMHDR
   {
      size_t param;
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
      void sendCompilationEndEvent(CompletionEvent* event, WindowApp* app);
      void sendErrorListSelEvent(SelectionEvent* event, WindowApp* app);
      void sendProjectViewSelectionEvent(ParamSelectionEvent* event, WindowApp* app);
      void sendLayoutEvent(LayoutEvent* event, WindowApp* app);
      void sendStartUpEvent(StartUpEvent* event, WindowApp* app);
      void sendTextContextMenuEvent(ContextMenuEvent* event, WindowApp* app);
      void sendBrowseContextMenuEvent(BrowseEvent* event, WindowApp* app);
      void sendSimpleEvent(SimpleEvent* event, WindowApp* app);

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
      IDESettings       ideSettingsDialog;
      DebuggerSettings  debuggerSettingsDialog;

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
      void onTextFrameSel(SelectionNMHDR* rec);
      void onProjectViewSel(ParamSelectionNMHDR* rec);
      void onIDEStatusChange(ModelNMHDR* rec);
      void onStartup(ModelNMHDR* rec);
      void onLayoutChange();
      void onDocumentSelection();

      void onDebugStep();
      void onDebugWatch();
      void onDebugEnd();

      void onDoubleClick(NMHDR* hdr);
      void onRClick(NMHDR* hdr);
      void onDebugWatchRClick(size_t index);
      void onContextMenu(ContextMenuNMHDR* rec);

      void onTabSelChanged(HWND wnd);
      void onTreeSelChanged(HWND wnd);
      void onChildRefresh(int controlId);
      void onTreeItemExpanded(NMTREEVIEW* rec);

      bool isTabToolTip(HWND handle);

      void onToolTip(NMTTDISPINFO* toolTip);
      void onTabTip(NMTTDISPINFO* toolTip);

      bool onCommand(int command) override;
      void onNotify(NMHDR* hdr) override;
      void onActivate() override;
      bool onClose() override;

      void onComilationStart();
      void onCompilationEnd(int exitCode, int postponedAction);
      void onErrorHighlight(int index);
      void onDebugWatchBrowse(BrowseNMHDR* rec);

      void updateCompileMenu(bool compileEnable, bool debugEnable, bool stopEnable);

      void onProjectChange(bool empty);
      void onProjectRefresh(bool empty);
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
      void excludeFile();

      void openHelp();
      void showAbout();

      void refreshDebugNode();
      void switchHexMode();

      void onDebuggerStart();
      void onDebuggerSourceNotFound();
      void onDocumentUpdate(DocumentChangeStatus& changeStatus);

   public:
      void populate(size_t counter, GUIControlBase** children) override;

      IDEWindow(wstr_t title, IDEController* controller, IDEModel* model, HINSTANCE instance, ViewFactoryBase* viewFactory);
   };

}

#endif