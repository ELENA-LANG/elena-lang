//---------------------------------------------------------------------------
//		E L E N A   P r o j e c t:  ELENA IDE
//                     WinAPI IDE Window Header File
//                                             (C)2021-2022, by Aleksey Rakov
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
      bool copyToClipboard(DocumentView* docView) override;
      void pasteFromClipboard(DocumentView* docView) override;

      Clipboard(ControlBase* owner);
   };

   // --- IDEWindow ---
   class IDEWindow : public SDIWindow, public DocumentNotifier
   {
      struct IDESttus
      {
         bool hasDocument;
         bool hasProject;
         bool running;
         bool hasSelection;
         bool canUndo;
         bool canRedo;
      };


      Dialog         fileDialog;
      Dialog         projectDialog;
      Clipboard      clipboard;

      HINSTANCE      _instance;

      IDEModel*      _model;
      IDEController* _controller;

      IDESttus       _ideStatus;

      void onStatusChange(StatusNMHDR* rec);
      //void onModelChange(ExtNMHDR* hdr);
      //void onNotifyMessage(ExtNMHDR* hdr);

      void onDebugWatch();

      void onDoubleClick(NMHDR* hdr);

      void onTabSelChanged(HWND wnd);
      void onTreeSelChanged(HWND wnd);
      void onChildRefresh(int controlId);

      void onLayoutChange(NotificationStatus status);
      void onIDEChange(NotificationStatus status);

      bool onCommand(int command) override;
      void onNotify(NMHDR* hdr) override;
      void onActivate() override;
      bool onClose() override;

      void onComilationStart();
      void onCompilationEnd(int exitCode);
      void onErrorHighlight(int index);

      void onProjectChange(bool empty);
      void onProjectViewSel(size_t index);

      void toggleTabBarWindow(int child_id);
      void toggleWindow(int child_id);

      void toggleProjectView(bool open);
      void openResultTab(int controlIndex);
      void setChildFocus(int controlIndex);

      void newFile();
      void openFile();
      void saveFile();
      void closeFile();
      void openProject();
      void closeProject();
      void exit();

      void undo();
      void redo();
      bool copyToClipboard();
      void pasteFromClipboard();
      void deleteText();
      void commentText();

      void openHelp();

      void onIDEViewUpdate(bool forced);
      void onDebuggerUpdate(bool running);

   public:
      void onDocumentUpdate() override;

      IDEWindow(wstr_t title, IDEController* controller, IDEModel* model, HINSTANCE instance);
   };

}

#endif